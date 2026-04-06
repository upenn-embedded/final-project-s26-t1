{ pkgs, ... }:
{
  boot.kernelPackages = pkgs.linuxPackages_rpi4;
  boot.initrd.allowMissingModules = true; # For some reason, dw-hdmi modules
}
