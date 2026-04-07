{pkgs, ...}:
{
  # Wayland Compositor
  programs.niri.enable = true;
  programs.niri.package = pkgs.niri-wrapped;

  # Launch niri automatically on TTY1
  environment.loginShellInit = ''
    [[ "$(tty)" == /dev/tty1 ]] && niri
  '';
}
