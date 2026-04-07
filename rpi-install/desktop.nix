{pkgs, ...}:
{
  # Wayland Compositor
  programs.niri.enable = true;

  # Launch niri automatically on TTY1
  environment.loginShellInit = ''
    [[ "$(tty)" == /dev/tty1 ]] && niri
  '';
}
