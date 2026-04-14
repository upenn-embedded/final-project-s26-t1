This is the code we use to create the image to flash onto the Raspberry Pi Model 4 B

# Plan
Use NixOS to create a reproducible image

# Installation Instructions
1. Clone this repository: `git clone https://github.com/upenn-embedded/final-project-s26-t1.git`
2. cd into the repository: `cd final-project-s26-t1`
3. ensure you have [nix](https://nixos.org/download/) installed
4. build the image: `nix build .#packages.aarch64-linux.rpi4B-img` (you may have to enable cross-compiling for `aarch64-linux` - I'm on NixOS with cross compilation enabled in my system config, so it just works for me)
5. plug-in the SD card into your system
6. Figure out which `disk` you need to flash (BE CAREFUL): `lsblk`
```
NAME                                          MAJ:MIN RM   SIZE RO TYPE  MOUNTPOINTS
sda                                             8:0    1  58.3G  0 disk
├─sda1                                          8:1    1   512M  0 part
└─sda2                                          8:2    1  57.8G  0 part
nvme0n1                                       259:0    0 931.5G  0 disk
├─nvme0n1p1                                   259:1    0 443.6G  0 part
│ └─luks-c705edf8-6321-4956-8780-f6c00a62e77e 254:0    0 443.6G  0 crypt /home
├─nvme0n1p5                                   259:2    0  96.8G  0 part
├─nvme0n1p6                                   259:3    0   512M  0 part  /boot
└─nvme0n1p7                                   259:4    0 390.6G  0 part  /nix/store`
```
7. In my case, the disk I need to flash is `sda`, which I can tell based off the size (64GB microSD card)
8. flash the SD card: `sudo dd if=result/sd-image/$(ls result/sd-image/) of=/dev/sdX bs=4M status=progress oflag=direct` (replace `sdX` with the drive you determined you need to flash). BE CAREFUL TO NOT OVERWRITE ANYTHING YOU WANT TO KEEP
9. put the SD card back into the Raspberry Pi

# Post-Install State
 * Automatically connects to AirPennNet-Device (you will have to register your device's MAC address to get it authorized to the network - you can find you mac address with `ip link` and look for the one associated with `wlan0`)
 * Automatically logs into all TTYs as `root`
 * `root` password is secret, but is already pre-set
 * OpenSSH open on all network interfaces with password login into `root`
 * Some utility packages pre-installed like `htop`, `neovim`, and `fastfetch`
 * Publishes itself to the local network through `avahi` (which you may also have to install) as `etch-a-sketch.local`

# Development Tips
* **connect via ssh**: `ssh root@etch-a-sketch.local`
* **"remember" etch-a-sketch password**: `ssh-copy-id root@etch-a-sketch.local`
* **update configuration remotely without re-flashing**
    * NixOS Machines: `nixos-rebuild switch --flake .#etch-a-sketch --target-host root@etch-a-sketch.local`
        * It can sometimes be faster to also build on the Pi so you don't have to cross-compile: `nixos-rebuild switch --flake .#etch-a-sketch --target-host root@etch-a-sketch.local --build-host root@etch-a-sketch.local`
    * idk: https://github.com/NixOS/nixpkgs/issues/44135
* **test desktop configuration locally** (Wayland only): `nix run .#niri-wrapped` (nicer if go to different TTY: `Ctrl+Alt+F#`)
