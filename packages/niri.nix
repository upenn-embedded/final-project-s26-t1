# Nix Settings Format: https://birdeehub.github.io/nix-wrapper-modules/wrapperModules/niri.html
# Niri Config Wiki: https://github.com/niri-wm/niri/wiki/Getting-Started
{pkgs, ...}:
{
  settings = {
    binds = {
      "Mod+Shift+E" = {
        "quit" = [];
      };
      "Mod+Return" = {
        "spawn" = [
          "alacritty"
        ];
      };
      "Mod+Q" = {
        "close-window" = [];
      };
      "Mod+F" = {
        "maximize-window-to-edges" = [];
      };
      "Mod+A" = {
        "focus-column-left" = [];
      };
      "Mod+D" = {
        "focus-column-right" = [];
      };
      "Mod+Shift+A" = {
        "move-column-left" = [];
      };
      "Mod+Shift+D" = {
        "move-column-right" = [];
      };
      "Mod+H" = {
        "show-hotkey-overlay" = [];
      };
    };

    layout = {
      background-color = "#bfbfc1";
    };

    gestures = {
      hot-corners.off = [];
    };

    cursor = {
      xcursor-theme = "Yaru";
      xcursor-size = 8;
    };

    spawn-sh-at-startup = [
      "eww daemon && eww open draw"
    ];
  };

  prefixVar =
  let
    # Programs that must be available to Niri environment
    envPackages = [
      pkgs.alacritty
      pkgs.eww-wrapped
    ]; 
  in
  ([
    # Make cursor theme discoverable
    [
      "XCURSOR_PATH"
      ":"
      "${pkgs.yaru-theme}/share/icons"
    ]
    [
      "PATH"
      ":"
      "${pkgs.lib.makeBinPath envPackages}"
    ]
  ]);
}
