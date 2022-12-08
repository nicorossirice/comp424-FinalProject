#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xfa12b18c, "module_layout" },
	{ 0x350ed57f, "param_ops_ulong" },
	{ 0xf0fda7ad, "platform_driver_unregister" },
	{ 0xbde95e6d, "__platform_driver_register" },
	{ 0x1747be18, "gpiod_set_debounce" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0xd36c5d64, "gpiod_to_irq" },
	{ 0x6be88ee4, "devm_gpiod_get" },
	{ 0x977f511b, "__mutex_init" },
	{ 0x58a197f2, "device_create" },
	{ 0xb31f6ca6, "__class_create" },
	{ 0x989307bb, "__register_chrdev" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0xf21017d9, "mutex_trylock" },
	{ 0x409bcb62, "mutex_unlock" },
	{ 0xb43f9365, "ktime_get" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x5b248560, "class_destroy" },
	{ 0xcc10bfa9, "class_unregister" },
	{ 0xea4da940, "device_destroy" },
	{ 0xc1514a3b, "free_irq" },
	{ 0xc5850110, "printk" },
};

MODULE_INFO(depends, "");

