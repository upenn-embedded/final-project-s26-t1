{ inputs, ... }:
{
  imports = [
    inputs.nix-hardware.nixosModules.raspberry-pi-4
  ];

  hardware.raspberry-pi."4".fkms-3d.enable = true;

  boot.initrd.allowMissingModules = true; # For some reason, dw-hdmi modules
}
