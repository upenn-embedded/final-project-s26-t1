{pkgs, ...}:
{
  sdImage.compressImage = false;

  time.timeZone = "America/New_York";
  i18n.defaultLocale = "en_US.UTF-8";

  networking = {
    hostName = "etch-a-sketch";
    networkmanager = {
      enable = true;
      ensureProfiles.profiles.AirPennNet-Device = {
        connection = {
          id = "AirPennNet-Device";
          interface-name = "wlan0";
          type = "wifi";
          uuid = "03c8eb02-9191-47f3-a5ad-aed522c2e10c";
        };
        ipv4 = {
          method = "auto";
        };
        ipv6 = {
          addr-gen-mode = "default";
          method = "auto";
        };
        proxy = { };
        wifi = {
          mode = "infrastructure";
          ssid = "AirPennNet-Device";
        };
        wifi-security = {
          auth-alg = "open";
          key-mgmt = "wpa-psk";
          psk = "penn1740wifi";
        };
      };
    };
  };

  services.openssh = {
    enable = true;
    settings.PermitRootLogin = "yes";
  };

  # include this repo on device
  environment.etc."nixos".source = ../.;

  users.users.root.hashedPassword = "$y$j9T$vfWlA2GOwpoLw72VuZ/g..$pyshG8GdIFO4GiPlTN8heKK.pBCNrrL7ZUc1GYbXzd3";
  services.getty.autologinUser = "root";

  environment.systemPackages = with pkgs; [
    fastfetch
  ];

  system.stateVersion = "25.11"; 
}
