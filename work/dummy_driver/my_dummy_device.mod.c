#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

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



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xae7140fb, "cdev_add" },
	{ 0xfb3de43c, "class_create" },
	{ 0x773c4019, "device_create" },
	{ 0x91d81025, "cdev_del" },
	{ 0x0bc5fb0d, "unregister_chrdev_region" },
	{ 0x7c77f2d5, "class_destroy" },
	{ 0x89258034, "device_destroy" },
	{ 0xd272d446, "__fentry__" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0x9f222e1e, "alloc_chrdev_region" },
	{ 0x97235f8d, "cdev_init" },
	{ 0xab006604, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xae7140fb,
	0xfb3de43c,
	0x773c4019,
	0x91d81025,
	0x0bc5fb0d,
	0x7c77f2d5,
	0x89258034,
	0xd272d446,
	0xe8213e80,
	0xd272d446,
	0x9f222e1e,
	0x97235f8d,
	0xab006604,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"cdev_add\0"
	"class_create\0"
	"device_create\0"
	"cdev_del\0"
	"unregister_chrdev_region\0"
	"class_destroy\0"
	"device_destroy\0"
	"__fentry__\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"alloc_chrdev_region\0"
	"cdev_init\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "278F02FFE942F348379E537");
