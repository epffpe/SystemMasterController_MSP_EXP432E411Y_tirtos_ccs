/*
 * URLHandler.c
 *
 *  Created on: Jun 19, 2019
 *      Author: epenate
 */


#define __HTTPSERVER_URLHANDLER_URLHANDLER_GLOBAL
#include "includes.h"

#undef Log_error0
#undef Log_error1
#undef Log_print0
#undef Log_print1
#undef Log_print2
#undef Log_print3
#undef Log_print4

#include <ti/net/http/httpserver.h>
#include <ti/net/http/http.h>
#include <ti/net/http/logging.h>

#include "HTTPServer/urlsimple.h"
#include "HTTPServer/URLHandler/URLHandler.h"




/* Maximum length of a value in the database */
#define MAX_DB_ENTRY_LEN    (64)

/* Maximum number of key-value pairs the server can store */
#define DB_SIZE             (10)

#define ERR_BAD_INPUT       (-1)
#define ERR_DB_FULL         (-2)



const char g_ccURL_simpleHTML[];
const char g_ccURL_dummyText[];



tURLHandlerEntry g_psURLTable[] =
{
 {"/",              URL_index, "Test"},
 {"/index.html",    URL_index, "Test"},
 {"/home.html",     URL_home, "Test"},
 {"/test.html",     URL_testFilePost, "Test"},
 {"/upload.html",   URL_fileUploadPost, "Test"},
 {"/send/data",     URL_fileUploadPost, "Test"},
 {0,0,0}
};




int URL_home(URLHandler_Handle urlHandler, int method,
            const char * url, const char * urlArgs,
            int contentLength, int ssock)
{
    int status          = URLHandler_ENOTHANDLED;
    char *body          = NULL;         /* Body of HTTP response in process */
    char *contentType   = "text/plain"; /* default (text/plain), but can be overridden: text/html; charset=utf-8*/
//    char *retString     = NULL;         /* String retrieved from server */
//    char *inputKey      = NULL;         /* Name of value to store in server */
//    char *inputValue    = NULL;         /* Value to store in server */
//    char argsToParse[MAX_DB_ENTRY_LEN];
    int returnCode;                     /* HTTP response status code */
//    int retc;                           /* Error checking for internal funcs */

    body = "PATCH is not handled by any handlers on this server.";
    returnCode = HTTP_SC_METHOD_NOT_ALLOWED;
    status = URLHandler_ENOTHANDLED;

    if (method == URLHandler_GET)
    {
        body = "/get 'home.html': This is the resource requested.";
        returnCode = HTTP_SC_OK;
        status = URLHandler_EHANDLED;
    }



    if (status != URLHandler_ENOTHANDLED)
    {
        if (contentLength > 0)
        {
//            char *buf;

//            buf = malloc(contentLength);
            /* This is done to flush the socket */
//            (void) Ssock_recvall(ssock, buf, contentLength, 0);
//            free(buf);
        }

        HTTPServer_sendSimpleResponse(ssock, returnCode, contentType,
                body ? strlen(g_ccURL_dummyText) : 0, NULL);
//        HTTPServer_sendSimpleResponse(ssock, returnCode, contentType,
//                body ? strlen(body) : 0, NULL);
//        HTTPServer_sendSimpleResponse(ssock, returnCode, contentType,
//                body ? 2000*strlen(body) : 0, NULL);
        if (body) {
//            for (contentLength = 0; contentLength < 2000; contentLength++) {
//                send(ssock, body, strlen(body), 0);
//            }
            send(ssock, g_ccURL_dummyText, strlen(g_ccURL_dummyText), 0);
//            send(ssock, body, strlen(body) - 1, 0);
//            sleep(5);
//            send(ssock, ".", 1, 0);
        }
//        HTTPServer_sendSimpleResponse(ssock, returnCode, contentType,
//                body ? strlen(body) : 0, body ? body : NULL);
    }

    return (status);
}

int URL_index(URLHandler_Handle urlHandler, int method,
            const char * url, const char * urlArgs,
            int contentLength, int ssock)
{
    int status          = URLHandler_ENOTHANDLED;
    char *body          = NULL;         /* Body of HTTP response in process */
    char *contentType   = "text/html; charset=utf-8"; /* default (text/plain), but can be overridden: text/html; charset=utf-8*/
//    char *retString     = NULL;         /* String retrieved from server */
//    char *inputKey      = NULL;         /* Name of value to store in server */
//    char *inputValue    = NULL;         /* Value to store in server */
//    char argsToParse[MAX_DB_ENTRY_LEN];
    int returnCode;                     /* HTTP response status code */
//    int retc;                           /* Error checking for internal funcs */

    body = "PATCH is not handled by any handlers on this server.";
    returnCode = HTTP_SC_METHOD_NOT_ALLOWED;
    status = URLHandler_ENOTHANDLED;

    if (method == URLHandler_GET)
    {
        body = "/get 'home.html': This is the resource requested.";
        returnCode = HTTP_SC_OK;
        status = URLHandler_EHANDLED;
    }



    if (status != URLHandler_ENOTHANDLED)
    {
        if (contentLength > 0)
        {
//            char *buf;

//            buf = malloc(contentLength);
            /* This is done to flush the socket */
//            (void) Ssock_recvall(ssock, buf, contentLength, 0);
//            free(buf);
        }

        HTTPServer_sendSimpleResponse(ssock, returnCode, contentType,
                body ? strlen(g_ccURL_simpleHTML) : 0, NULL);
//        HTTPServer_sendSimpleResponse(ssock, returnCode, contentType,
//                body ? strlen(body) : 0, NULL);
//        HTTPServer_sendSimpleResponse(ssock, returnCode, contentType,
//                body ? 2000*strlen(body) : 0, NULL);
        if (body) {
//            for (contentLength = 0; contentLength < 2000; contentLength++) {
//                send(ssock, body, strlen(body), 0);
//            }
            send(ssock, g_ccURL_simpleHTML, strlen(g_ccURL_simpleHTML), 0);
//            send(ssock, body, strlen(body) - 1, 0);
//            sleep(5);
//            send(ssock, ".", 1, 0);
        }
//        HTTPServer_sendSimpleResponse(ssock, returnCode, contentType,
//                body ? strlen(body) : 0, body ? body : NULL);
    }

    return (status);
}


int URL_testFilePost(URLHandler_Handle urlHandler, int method,
            const char * url, const char * urlArgs,
            int contentLength, int ssock)
{
    int status          = URLHandler_ENOTHANDLED;
    char *body          = NULL;         /* Body of HTTP response in process */
    char *contentType   = "text/plain"; /* default (text/plain), but can be overridden: text/html; charset=utf-8*/
//    char *retString     = NULL;         /* String retrieved from server */
//    char *inputKey      = NULL;         /* Name of value to store in server */
//    char *inputValue    = NULL;         /* Value to store in server */
//    char argsToParse[MAX_DB_ENTRY_LEN];
    int returnCode;                     /* HTTP response status code */
//    int retc;                           /* Error checking for internal funcs */

    body = "PATCH is not handled by any handlers on this server.";
    returnCode = HTTP_SC_METHOD_NOT_ALLOWED;
    status = URLHandler_ENOTHANDLED;

    if (method == URLHandler_GET)
    {
        body = "/get 'URL_testFilePost': This is the resource requested.";
        returnCode = HTTP_SC_OK;
        status = URLHandler_EHANDLED;
    }

    if (method == URLHandler_POST)
    {
        body = "/post 'URL_testFilePost': This is the resource requested.";
        returnCode = HTTP_SC_OK;

        vEFS_setFlashDataFileNameHTTP(ssock, contentLength, "test");
        HTTPServer_sendSimpleResponse(ssock, returnCode, contentType,
                        body ? strlen(body) : 0, body);
    }



    if (status != URLHandler_ENOTHANDLED)
    {
        if (contentLength > 0)
        {
//            char *buf;

//            buf = malloc(contentLength);
            /* This is done to flush the socket */
//            (void) Ssock_recvall(ssock, buf, contentLength, 0);
//            free(buf);
        }

        HTTPServer_sendSimpleResponse(ssock, returnCode, contentType,
                body ? strlen(g_ccURL_dummyText) : 0, NULL);
//        HTTPServer_sendSimpleResponse(ssock, returnCode, contentType,
//                body ? strlen(body) : 0, NULL);
//        HTTPServer_sendSimpleResponse(ssock, returnCode, contentType,
//                body ? 2000*strlen(body) : 0, NULL);
        if (body) {
//            for (contentLength = 0; contentLength < 2000; contentLength++) {
//                send(ssock, body, strlen(body), 0);
//            }
            send(ssock, g_ccURL_dummyText, strlen(g_ccURL_dummyText), 0);
//            send(ssock, body, strlen(body) - 1, 0);
//            sleep(5);
//            send(ssock, ".", 1, 0);
        }
//        HTTPServer_sendSimpleResponse(ssock, returnCode, contentType,
//                body ? strlen(body) : 0, body ? body : NULL);
    }

    return (status);
}


int URL_fileUploadPost(URLHandler_Handle urlHandler, int method,
            const char * url, const char * urlArgs,
            int contentLength, int ssock)
{
    int status          = URLHandler_ENOTHANDLED;
    char *body          = NULL;         /* Body of HTTP response in process */
    char *contentType   = "text/plain"; /* default (text/plain), but can be overridden: text/html; charset=utf-8*/
//    char *retString     = NULL;         /* String retrieved from server */
//    char *inputKey      = NULL;         /* Name of value to store in server */
//    char *inputValue    = NULL;         /* Value to store in server */
//    char argsToParse[MAX_DB_ENTRY_LEN];
    int returnCode;                     /* HTTP response status code */
//    int retc;                           /* Error checking for internal funcs */

    body = "PATCH is not handled by any handlers on this server.";
    returnCode = HTTP_SC_METHOD_NOT_ALLOWED;
    status = URLHandler_ENOTHANDLED;

    if (method == URLHandler_GET)
    {
        body = "/get 'URL_testFilePost': This is the resource requested.";
        returnCode = HTTP_SC_OK;
        vEFS_getFlashDataFileNameHTTP(ssock, "index.html");
        status = URLHandler_EHANDLED;
        return status;
    }

    if (method == URLHandler_POST)
    {
        body = "/post 'URL_testFilePost': This is the resource requested.";
        returnCode = HTTP_SC_OK;

        vEFS_setFlashDataFileNameHTTP(ssock, contentLength, "index.html");
        HTTPServer_sendSimpleResponse(ssock, returnCode, contentType,
                        body ? strlen(body) : 0, body);
    }



    if (status != URLHandler_ENOTHANDLED)
    {
        if (contentLength > 0)
        {
//            char *buf;

//            buf = malloc(contentLength);
            /* This is done to flush the socket */
//            (void) Ssock_recvall(ssock, buf, contentLength, 0);
//            free(buf);
        }

        HTTPServer_sendSimpleResponse(ssock, returnCode, contentType,
                body ? strlen(g_ccURL_dummyText) : 0, NULL);
//        HTTPServer_sendSimpleResponse(ssock, returnCode, contentType,
//                body ? strlen(body) : 0, NULL);
//        HTTPServer_sendSimpleResponse(ssock, returnCode, contentType,
//                body ? 2000*strlen(body) : 0, NULL);
        if (body) {
//            for (contentLength = 0; contentLength < 2000; contentLength++) {
//                send(ssock, body, strlen(body), 0);
//            }
            send(ssock, g_ccURL_dummyText, strlen(g_ccURL_dummyText), 0);
//            send(ssock, body, strlen(body) - 1, 0);
//            sleep(5);
//            send(ssock, ".", 1, 0);
        }
//        HTTPServer_sendSimpleResponse(ssock, returnCode, contentType,
//                body ? strlen(body) : 0, body ? body : NULL);
    }

    return (status);
}


const char g_ccURL_simpleHTML[] =
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<title>System Master Controller</title>\n"
//        "<meta http-equiv=\"refresh\" content=\"10\">\n"
        "</head>\n"
        "<body>\n"

        "<h1>My First Heading</h1>\n"
        "<p>My first paragraph.</p>\n"
        "<a href=\"/home.html\">This is a link to a long file</a>\n"
        "<br>\n"
        "<button>Click me</button>\n"
        "<ul>\n"
        "  <li>Coffee</li>\n"
        "  <li>Tea</li>\n"
        "  <li>Milk</li>\n"
        "</ul>\n"

        "<ol>\n"
        "  <li>Coffee</li>\n"
        "  <li>Tea</li>\n"
        "  <li>Milk</li>\n"

        "<br>\n"

        "<img src=https://altoaviation.com/wp-content/uploads/2019/03/ALTO-logo.png>\n"
        "</ol>\n"
        "</body>\n"
        "</html>\n";

const char g_ccURL_dummyText[] =
        "load mmc 0:2 ${loadaddr} /boot/uImage;load mmc 0:2 ${fdtaddr} /boot/am335x-boneblack.dtb\n"
        "setenv bootargs \"console=${console} root=/dev/mmcblk0p2 ro\"\n"
        "bootm ${loadaddr} - ${fdtaddr}\n\n"

        "=> printenv\n"
        "arch=arm\n"
        "args_mmc=run finduuid;setenv bootargs console=${console} ${optargs} ${cape_disable} ${cape_enable} root=PARTUUID=${uuid} ro rootfstype=${mmcrootfstype} ${cmdline}\n"
        "args_mmc_old=setenv bootargs console=${console} ${optargs} ${cape_disable} ${cape_enable} root=${oldroot} ro rootfstype=${mmcrootfstype} ${cmdline}\n"
        "args_mmc_uuid=setenv bootargs console=${console} ${optargs} ${cape_disable} ${cape_enable} root=UUID=${uuid} ro rootfstype=${mmcrootfstype} ${cmdline}\n"
        "args_netinstall=setenv bootargs ${netinstall_bootargs} ${optargs} ${cape_disable} ${cape_enable} root=/dev/ram rw ${cmdline}\n"
        "args_uenv_root=setenv bootargs console=${console} ${optargs} ${cape_disable} ${cape_enable} root=${uenv_root} ro rootfstype=${mmcrootfstype} ${cmdline}\n"
        "autoconf=off\n"
        "baudrate=115200\n"
        "board=am335x\n"
        "board_name=A335BNLT\n"
        "board_rev=000C\n"
        "boot=${interface} dev ${mmcdev}; if ${interface} rescan; then gpio set 54;setenv bootpart ${mmcdev}:1; if test -e ${interface} ${bootpart} /etc/fstab; then setenv mmcpart 1;fi; echo Checking for: /uEnv.txt ...;if test -e ${interface} ${bootpart} /uEnv.txt; then if run loadbootenv; then gpio set 55;echo Loaded environment from ${bootenv};run importbootenv;fi;if test -n ${cape}; then if test -e ${interface} ${bootpart} ${fdtdir}/${fdtbase}-${cape}.dtb; then setenv fdtfile ${fdtbase}-${cape}.dtb; fi; echo using: $fdtfile...; fi; echo Checking if uenvcmd is set ...;if test -n ${uenvcmd}; then gpio set 56; echo Running uenvcmd ...;run uenvcmd;fi;echo Checking if client_ip is set ...;if test -n ${client_ip}; then if test -n ${dtb}; then setenv fdtfile ${dtb};echo using ${fdtfile} ...;fi;gpio set 56; if test -n ${uname_r}; then echo Running nfsboot_uname_r ...;run nfsboot_uname_r;fi;echo Running nfsboot ...;run nfsboot;fi;fi; echo Checking for: /${script} ...;if test -e ${interface} ${bootpart} /${script}; then gpio set 55;setenv scriptfile ${script};run loadbootscript;echo Loaded script from ${scriptfile};gpio set 56; run bootscript;fi; echo Checking for: /boot/${script} ...;if test -e ${interface} ${bootpart} /boot/${script}; then gpio set 55;setenv scriptfile /boot/${script};run loadbootscript;echo Loaded script from ${scriptfile};gpio set 56; run bootscript;fi; echo Checking for: /boot/uEnv.txt ...;for i in 1 2 3 4 5 6 7 ; do setenv mmcpart ${i};setenv bootpart ${mmcdev}:${mmcpart};if test -e ${interface} ${bootpart} /boot/uEnv.txt; then gpio set 55;load ${interface} ${bootpart} ${loadaddr} /boot/uEnv.txt;env import -t ${loadaddr} ${filesize};echo Loaded environment from /boot/uEnv.txt;if test -n ${cape}; then echo debug: [cape=${cape}] ... ;setenv fdtfile ${fdtbase}-${cape}.dtb; echo Using: dtb=${fdtfile} ...;fi; if test -n ${dtb}; then echo debug: [dtb=${dtb}] ... ;setenv fdtfile ${dtb};echo Using: dtb=${fdtfile} ...;fi;echo Checking if uname_r is set in /boot/uEnv.txt...;if test -n ${uname_r}; then gpio set 56; setenv oldroot /dev/mmcblk${mmcdev}p${mmcpart};echo Running uname_boot ...;run uname_boot;fi;fi;done;fi;\n"
        "boot_a_script=load ${devtype} ${devnum}:${distro_bootpart} ${scriptaddr} ${prefix}${script}; source ${scriptaddr}\n"
        "boot_extlinux=sysboot ${devtype} ${devnum}:${distro_bootpart} any ${scriptaddr} ${prefix}extlinux/extlinux.conf\n"
        "boot_fdt=try\n"
        "boot_prefixes=/ /boot/\n"
        "boot_script_dhcp=boot.scr.uimg\n"
        "boot_scripts=boot.scr.uimg boot.scr\n"
        "boot_targets=mmc0 legacy_mmc0 mmc1 legacy_mmc1 nand0 pxe dhcp\n"
        "bootcmd=run findfdt; run distro_bootcmd\n"
        "bootcmd_dhcp=if dhcp ${scriptaddr} ${boot_script_dhcp}; then source ${scriptaddr}; fi\n"
        "bootcmd_legacy_mmc0=gpio clear 56; gpio clear 55; gpio clear 54; gpio set 53; setenv interface mmc; setenv mmcdev 0; setenv bootpart 0:1 ; run boot\n"
        "bootcmd_legacy_mmc1=gpio clear 56; gpio clear 55; gpio clear 54; gpio set 53; setenv interface mmc; setenv mmcdev 1; setenv bootpart 1:1 ; run boot\n"
        "bootcmd_mmc0=setenv devnum 0; run mmc_boot\n"
        "bootcmd_mmc1=setenv devnum 1; run mmc_boot\n"
        "bootcmd_nand=run nandboot\n"
        "bootcmd_pxe=dhcp; if pxe get; then pxe boot; fi\n"
        "bootcount=3\n"
        "bootdelay=2\n"
        "bootdir=/boot\n"
        "bootenv=uEnv.txt\n"
        "bootfile=zImage\n"
        "bootm_size=0x10000000\n"
        "bootpart=0:2\n"
        "bootscript=echo Running bootscript from mmc${bootpart} ...; source ${loadaddr}\n"
        "console=ttyO0,115200n8\n"
        "cpu=armv7\n"
        "device=eth0\n"
        "dfu_alt_info_emmc=rawemmc raw 0 3751936\n"
        "dfu_alt_info_mmc=boot part 0 1;rootfs part 0 2;MLO fat 0 1;MLO.raw raw 0x100 0x100;u-boot.img.raw raw 0x300 0x400;spl-os-args.raw raw 0x80 0x80;spl-os-image.raw raw 0x900 0x2000;spl-os-args fat 0 1;spl-os-image fat 0 1;u-boot.img fat 0 1;uEnv.txt fat 0 1\n"
        "dfu_alt_info_ram=kernel ram 0x80200000 0xD80000;fdt ram 0x80F80000 0x80000;ramdisk ram 0x81000000 0x4000000\n"
        "distro_bootcmd=for target in ${boot_targets}; do run bootcmd_${target}; done\n"
        "eth1addr=7c:66:9d:58:ae:bb\n"
        "ethact=cpsw\n"
        "ethaddr=7c:66:9d:58:ae:b9\n"
        "failumsboot=echo; echo FAILSAFE: U-Boot UMS (USB Mass Storage) enabled, media now available over the usb slave port ...; ums 0 ${interface} 1;\n"
        "fdt_addr_r=0x88000000\n"
        "fdtaddr=0x88000000\n"
        "fdtfile=undefined\n"
        "findfdt=if test $board_name = A335BONE; then setenv fdtfile am335x-bone.dtb; setenv fdtbase am335x-bone; fi; if test $board_name = A335BNLT; then if test $board_rev = BLA0; then setenv fdtfile am335x-boneblue.dtb; setenv fdtbase am335x-boneblue; elif test $board_rev = BBG1; then setenv fdtfile am335x-bonegreen.dtb; setenv fdtbase am335x-bonegreen; elif test $board_rev = GW1A; then setenv fdtfile am335x-bonegreen-wireless.dtb; setenv fdtbase am335x-bonegreen-wireless; elif test $board_rev = AIA0; then setenv fdtfile am335x-abbbi.dtb; setenv fdtbase am335x-abbbi; elif test $board_rev = EIA0; then setenv fdtfile am335x-boneblack.dtb; setenv fdtbase am335x-boneblack; elif test $board_rev = SE0A; then setenv fdtfile am335x-sancloud-bbe.dtb; setenv fdtbase am335x-sancloud-bbe; elif test $board_rev = ME06; then setenv fdtfile am335x-bonegreen.dtb; setenv fdtbase am335x-bonegreen; elif test $board_rev = GH01; then setenv fdtfile am335x-boneblack.dtb; setenv fdtbase am335x-boneblack; else setenv fdtfile am335x-boneblack.dtb; setenv fdtbase am335x-boneblack; fi; fi; if test $board_name = A33515BB; then setenv fdtfile am335x-evm.dtb; fi; if test $board_name = A335X_SK; then setenv fdtfile am335x-evmsk.dtb; fi; if test $fdtfile = undefined; then echo WARNING: Could not determine device tree to use; fi;\n"
        "finduuid=part uuid ${interface} ${bootpart} uuid\n"
        "gw_ip=192.168.1.1\n"
        "importbootenv=echo Importing environment from ${interface} ...; env import -t -r $loadaddr $filesize\n"
        "kernel_addr_r=0x82000000\n"
        "loadaddr=0x82000000\n"
        "loadbootenv=load ${interface} ${bootpart} ${loadaddr} ${bootenv}\n"
        "loadbootscript=load ${interface} ${bootpart} ${loadaddr} ${scriptfile};\n"
        "loadfdt=echo loading ${fdtdir}/${fdtfile} ...; load ${interface} ${bootpart} ${fdtaddr} ${fdtdir}/${fdtfile}\n"
        "loadimage=load ${interface} ${bootpart} ${loadaddr} ${bootdir}/${bootfile}\n"
        "loadramdisk=load ${interface} ${mmcdev} ${rdaddr} ramdisk.gz\n"
        "loadrd=load ${interface} ${bootpart} ${rdaddr} ${bootdir}/${rdfile}; setenv rdsize ${filesize}\n"
        "mmc_boot=if mmc dev ${devnum}; then setenv devtype mmc; run scan_dev_for_boot_part; fi\n"
        "mmcboot=${interface} dev ${mmcdev}; if ${interface} rescan; then echo SD/MMC found on device ${mmcdev};if run loadbootscript; then run bootscript;else if run loadbootenv; then echo Loaded environment from ${bootenv};run importbootenv;fi;if test -n $uenvcmd; then echo Running uenvcmd ...;run uenvcmd;fi;if run loadimage; then run mmcloados;fi;fi ;fi;\n"
        "mmcdev=0\n"
        "mmcloados=run args_mmc; if test ${boot_fdt} = yes || test ${boot_fdt} = try; then if run loadfdt; then bootz ${loadaddr} - ${fdtaddr}; else if test ${boot_fdt} = try; then bootz; else echo WARN: Cannot load the DT; fi; fi; else bootz; fi;\n"
        "mmcrootfstype=ext4 rootwait\n"
        "netargs=setenv bootargs console=${console} ${optargs} root=/dev/nfs nfsroot=${serverip}:${rootpath},${nfsopts} rw ip=dhcp\n"
        "netboot=echo Booting from network ...; setenv autoload no; dhcp; run netloadimage; run netloadfdt; run netargs; bootz ${loadaddr} - ${fdtaddr}\n"
        "netloadfdt=tftp ${fdtaddr} ${fdtfile}\n"
        "netloadimage=tftp ${loadaddr} ${bootfile}\n"
        "netmask=255.255.255.0\n"
        "nfs_options=,vers=3\n"
        "nfsargs=setenv bootargs console=${console} ${optargs} ${cape_disable} ${cape_enable} root=/dev/nfs rw rootfstype=${nfsrootfstype} nfsroot=${nfsroot} ip=${ip} ${cmdline}\n"
        "nfsboot=echo Booting from ${server_ip} ...; setenv nfsroot ${server_ip}:${root_dir}${nfs_options}; setenv ip ${client_ip}:${server_ip}:${gw_ip}:${netmask}:${hostname}:${device}:${autoconf}; setenv autoload no; setenv serverip ${server_ip}; setenv ipaddr ${client_ip}; tftp ${loadaddr} ${tftp_dir}${bootfile}; tftp ${fdtaddr} ${tftp_dir}dtbs/${fdtfile}; run nfsargs; bootz ${loadaddr} - ${fdtaddr}\n"
        "nfsboot_uname_r=echo Booting from ${server_ip} ...; setenv nfsroot ${server_ip}:${root_dir}${nfs_options}; setenv ip ${client_ip}:${server_ip}:${gw_ip}:${netmask}:${hostname}:${device}:${autoconf}; setenv autoload no; setenv serverip ${server_ip}; setenv ipaddr ${client_ip}; tftp ${loadaddr} ${tftp_dir}vmlinuz-${uname_r}; tftp ${fdtaddr} ${tftp_dir}dtbs/${uname_r}/${fdtfile}; run nfsargs; bootz ${loadaddr} - ${fdtaddr}\n"
        "nfsopts=nolock\n"
        "nfsrootfstype=ext4 rootwait fixrtc\n"
        "partitions=uuid_disk=${uuid_gpt_disk};name=rootfs,start=2MiB,size=-,uuid=${uuid_gpt_rootfs}\n"
        "pxefile_addr_r=0x80100000\n"
        "ramargs=setenv bootargs console=${console} ${optargs} root=${ramroot} rootfstype=${ramrootfstype}\n"
        "ramboot=echo Booting from ramdisk ...; run ramargs; bootz ${loadaddr} ${rdaddr} ${fdtaddr}\n"
        "ramdisk_addr_r=0x88080000\n"
        "ramroot=/dev/ram0 rw\n"
        "ramrootfstype=ext2\n"
        "rdaddr=0x88080000\n"
        "root_dir=/home/userid/targetNFS\n"
        "rootpath=/export/rootfs\n"
        "scan_dev_for_boot=echo Scanning ${devtype} ${devnum}:${distro_bootpart}...; for prefix in ${boot_prefixes}; do run scan_dev_for_extlinux; run scan_dev_for_scripts; done\n"
        "scan_dev_for_boot_part=part list ${devtype} ${devnum} -bootable devplist; env exists devplist || setenv devplist 1; for distro_bootpart in ${devplist}; do if fstype ${devtype} ${devnum}:${distro_bootpart} bootfstype; then run scan_dev_for_boot; fi; done\n"
        "scan_dev_for_extlinux=if test -e ${devtype} ${devnum}:${distro_bootpart} ${prefix}extlinux/extlinux.conf; then echo Found ${prefix}extlinux/extlinux.conf; run boot_extlinux; echo SCRIPT FAILED: continuing...; fi\n"
        "scan_dev_for_scripts=for script in ${boot_scripts}; do if test -e ${devtype} ${devnum}:${distro_bootpart} ${prefix}${script}; then echo Found U-Boot script ${prefix}${script}; run boot_a_script; echo SCRIPT FAILED: continuing...; fi; done\n"
        "script=boot.scr\n"
        "scriptaddr=0x80000000\n"
        "scriptfile=${script}\n"
        "server_ip=192.168.1.100\n"
        "soc=am33xx\n"
        "spiargs=setenv bootargs console=${console} ${optargs} root=${spiroot} rootfstype=${spirootfstype}\n"
        "spiboot=echo Booting from spi ...; run spiargs; sf probe ${spibusno}:0; sf read ${loadaddr} ${spisrcaddr} ${spiimgsize}; bootz ${loadaddr}\n"
        "spibusno=0\n"
        "spiimgsize=0x362000\n"
        "spiroot=/dev/mtdblock4 rw\n"
        "spirootfstype=jffs2\n"
        "spisrcaddr=0xe0000\n"
        "static_ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}::off\n"
        "stderr=ns16550_serial\n"
        "stdin=ns16550_serial\n"
        "stdout=ns16550_serial\n"
        "uname_boot=setenv bootdir /boot; setenv bootfile vmlinuz-${uname_r}; if test -e ${interface} ${bootpart} ${bootdir}/${bootfile}; then echo loading ${bootdir}/${bootfile} ...; run loadimage;setenv fdtdir /boot/dtbs/${uname_r}; if test -e ${interface} ${bootpart} ${fdtdir}/${fdtfile}; then run loadfdt;else setenv fdtdir /usr/lib/linux-image-${uname_r}; if test -e ${interface} ${bootpart} ${fdtdir}/${fdtfile}; then run loadfdt;else setenv fdtdir /lib/firmware/${uname_r}/device-tree; if test -e ${interface} ${bootpart} ${fdtdir}/${fdtfile}; then run loadfdt;else setenv fdtdir /boot/dtb-${uname_r}; if test -e ${interface} ${bootpart} ${fdtdir}/${fdtfile}; then run loadfdt;else setenv fdtdir /boot/dtbs; if test -e ${interface} ${bootpart} ${fdtdir}/${fdtfile}; then run loadfdt;else setenv fdtdir /boot/dtb; if test -e ${interface} ${bootpart} ${fdtdir}/${fdtfile}; then run loadfdt;else setenv fdtdir /boot; if test -e ${interface} ${bootpart} ${fdtdir}/${fdtfile}; then run loadfdt;else if test -e ${interface} ${bootpart} ${fdtfile}; then run loadfdt;else echo; echo unable to find [dtb=${fdtfile}] did you name it correctly? ...; run failumsboot;fi;fi;fi;fi;fi;fi;fi;fi; setenv rdfile initrd.img-${uname_r}; if test -e ${interface} ${bootpart} ${bootdir}/${rdfile}; then echo loading ${bootdir}/${rdfile} ...; run loadrd;if test -n ${netinstall_enable}; then run args_netinstall; run message;echo debug: [${bootargs}] ... ;echo debug: [bootz ${loadaddr} ${rdaddr}:${rdsize} ${fdtaddr}] ... ;bootz ${loadaddr} ${rdaddr}:${rdsize} ${fdtaddr}; fi;if test -n ${uenv_root}; then run args_uenv_root;echo debug: [${bootargs}] ... ;echo debug: [bootz ${loadaddr} ${rdaddr}:${rdsize} ${fdtaddr}] ... ;bootz ${loadaddr} ${rdaddr}:${rdsize} ${fdtaddr}; fi;if test -n ${uuid}; then run args_mmc_uuid;echo debug: [${bootargs}] ... ;echo debug: [bootz ${loadaddr} ${rdaddr}:${rdsize} ${fdtaddr}] ... ;bootz ${loadaddr} ${rdaddr}:${rdsize} ${fdtaddr}; fi;run args_mmc_old;echo debug: [${bootargs}] ... ;echo debug: [bootz ${loadaddr} ${rdaddr}:${rdsize} ${fdtaddr}] ... ;bootz ${loadaddr} ${rdaddr}:${rdsize} ${fdtaddr}; else if test -n ${uenv_root}; then run args_uenv_root;echo debug: [${bootargs}] ... ;echo debug: [bootz ${loadaddr} - ${fdtaddr}] ... ;bootz ${loadaddr} - ${fdtaddr}; fi;run args_mmc_old;echo debug: [${bootargs}] ... ;echo debug: [bootz ${loadaddr} - ${fdtaddr}] ... ;bootz ${loadaddr} - ${fdtaddr}; fi;fi;\n"
        "usbnet_devaddr=7c:66:9d:58:ae:bb\n"
        "vendor=ti\n"
        "ver=U-Boot 2016.01-00001-g55c8ec0 (Mar 09 2016 - 09:36:54 -0600)\n\n"

        "Environment size: 13831/131068 bytes\n";
