/*
 *  Copyright (C) 2014 Linaro Ltd
 *
 * Author: Ulf Hansson <ulf.hansson@linaro.org>
 *
 * License terms: GNU General Public License (GPL) version 2
 *
 *  Simple MMC power sequence management
 */
#include <linux/version.h>
#include <linux/clk.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/of_gpio.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0))
#include <linux/gpio/consumer.h>
#endif
#include <linux/mmc/host.h>

#include "pwrseq.h"


#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 1, 0))
/**
 * Opaque descriptor for a GPIO. These are obtained using gpiod_get() and are
 * preferable to the old integer-based handles.
 *
 * Contrary to integers, a pointer to a gpio_desc is guaranteed to be valid
 * until the GPIO is released.
 */
struct gpio_desc;

#define GPIOD_FLAGS_BIT_DIR_SET		BIT(0)
#define GPIOD_FLAGS_BIT_DIR_OUT		BIT(1)
#define GPIOD_FLAGS_BIT_DIR_VAL		BIT(2)

/**
 * Optional flags that can be passed to one of gpiod_* to configure direction
 * and output value. These values cannot be OR'd.
 */
enum gpiod_flags {
	GPIOD_ASIS	= 0,
	GPIOD_IN	= GPIOD_FLAGS_BIT_DIR_SET,
	GPIOD_OUT_LOW	= GPIOD_FLAGS_BIT_DIR_SET | GPIOD_FLAGS_BIT_DIR_OUT,
	GPIOD_OUT_HIGH	= GPIOD_FLAGS_BIT_DIR_SET | GPIOD_FLAGS_BIT_DIR_OUT |
			  GPIOD_FLAGS_BIT_DIR_VAL,
};

static void gpiod_set_value_cansleep(struct gpio_desc *desc, int value)
{
	/* GPIO can never have been requested */
	WARN_ON(1);
}

/* Acquire and dispose GPIOs */
static struct gpio_desc *__must_check gpiod_get_index(struct device *dev,
					       const char *con_id,
					       unsigned int idx,
					       enum gpiod_flags flags)
{
	return ERR_PTR(-ENOSYS);
}

static void gpiod_put(struct gpio_desc *desc)
{
	might_sleep();

	/* GPIO can never have been requested */
	WARN_ON(1);
}
#endif

struct mmc_pwrseq_simple {
	struct mmc_pwrseq pwrseq;
	bool clk_enabled;
	struct clk *ext_clk;
	int nr_gpios;
	struct gpio_desc *reset_gpios[0];
};

static void mmc_pwrseq_simple_set_gpios_value(struct mmc_pwrseq_simple *pwrseq,
					      int value)
{
	int i;

	for (i = 0; i < pwrseq->nr_gpios; i++)
		if (!IS_ERR(pwrseq->reset_gpios[i]))
			gpiod_set_value_cansleep(pwrseq->reset_gpios[i], value);
}

static void mmc_pwrseq_simple_pre_power_on(struct mmc_host *host)
{
	struct mmc_pwrseq_simple *pwrseq = container_of(host->pwrseq,
					struct mmc_pwrseq_simple, pwrseq);

	if (!IS_ERR(pwrseq->ext_clk) && !pwrseq->clk_enabled) {
		clk_prepare_enable(pwrseq->ext_clk);
		pwrseq->clk_enabled = true;
	}

	mmc_pwrseq_simple_set_gpios_value(pwrseq, 1);
}

static void mmc_pwrseq_simple_post_power_on(struct mmc_host *host)
{
	struct mmc_pwrseq_simple *pwrseq = container_of(host->pwrseq,
					struct mmc_pwrseq_simple, pwrseq);

	mmc_pwrseq_simple_set_gpios_value(pwrseq, 0);
}

static void mmc_pwrseq_simple_power_off(struct mmc_host *host)
{
	struct mmc_pwrseq_simple *pwrseq = container_of(host->pwrseq,
					struct mmc_pwrseq_simple, pwrseq);

	mmc_pwrseq_simple_set_gpios_value(pwrseq, 1);

	if (!IS_ERR(pwrseq->ext_clk) && pwrseq->clk_enabled) {
		clk_disable_unprepare(pwrseq->ext_clk);
		pwrseq->clk_enabled = false;
	}
}

static void mmc_pwrseq_simple_free(struct mmc_host *host)
{
	struct mmc_pwrseq_simple *pwrseq = container_of(host->pwrseq,
					struct mmc_pwrseq_simple, pwrseq);
	int i;

	for (i = 0; i < pwrseq->nr_gpios; i++)
		if (!IS_ERR(pwrseq->reset_gpios[i]))
			gpiod_put(pwrseq->reset_gpios[i]);

	if (!IS_ERR(pwrseq->ext_clk))
		clk_put(pwrseq->ext_clk);

	kfree(pwrseq);
}

static struct mmc_pwrseq_ops mmc_pwrseq_simple_ops = {
	.pre_power_on = mmc_pwrseq_simple_pre_power_on,
	.post_power_on = mmc_pwrseq_simple_post_power_on,
	.power_off = mmc_pwrseq_simple_power_off,
	.free = mmc_pwrseq_simple_free,
};

struct mmc_pwrseq *mmc_pwrseq_simple_alloc(struct mmc_host *host,
					   struct device *dev)
{
	struct mmc_pwrseq_simple *pwrseq;
	int i, nr_gpios, ret = 0;

	nr_gpios = of_gpio_named_count(dev->of_node, "reset-gpios");
	if (nr_gpios < 0)
		nr_gpios = 0;

	pwrseq = kzalloc(sizeof(struct mmc_pwrseq_simple) + nr_gpios *
			 sizeof(struct gpio_desc *), GFP_KERNEL);
	if (!pwrseq)
		return ERR_PTR(-ENOMEM);

	pwrseq->ext_clk = clk_get(dev, "ext_clock");
	if (IS_ERR(pwrseq->ext_clk) &&
	    PTR_ERR(pwrseq->ext_clk) != -ENOENT) {
		ret = PTR_ERR(pwrseq->ext_clk);
		goto free;
	}

	for (i = 0; i < nr_gpios; i++) {
		pwrseq->reset_gpios[i] = gpiod_get_index(dev, "reset", i,
							 GPIOD_OUT_HIGH);
		if (IS_ERR(pwrseq->reset_gpios[i]) &&
		    PTR_ERR(pwrseq->reset_gpios[i]) != -ENOENT &&
		    PTR_ERR(pwrseq->reset_gpios[i]) != -ENOSYS) {
			ret = PTR_ERR(pwrseq->reset_gpios[i]);

			while (i--)
				gpiod_put(pwrseq->reset_gpios[i]);

			goto clk_put;
		}
	}

	pwrseq->nr_gpios = nr_gpios;
	pwrseq->pwrseq.ops = &mmc_pwrseq_simple_ops;

	return &pwrseq->pwrseq;
clk_put:
	if (!IS_ERR(pwrseq->ext_clk))
		clk_put(pwrseq->ext_clk);
free:
	kfree(pwrseq);
	return ERR_PTR(ret);
}
