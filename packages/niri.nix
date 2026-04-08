# Nix Settings Format: https://birdeehub.github.io/nix-wrapper-modules/wrapperModules/niri.html
# Niri Config Wiki: https://github.com/niri-wm/niri/wiki/Getting-Started
{pkgs, lib, ...}:
let
  getWidthBash = "wlr-randr | awk '/current/ {print $1; exit}' | cut -d'x' -f1";
  getHeightBash = "wlr-randr | awk '/current/ {print $1; exit}' | cut -d'x' -f2";
  getScale = "wlr-randr | awk '/Scale:/ {print $2; exit}'";
in
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
      "Mod+K" = {
        spawn-sh = "pkill -x wvkbd-mobintl || wvkbd-mobintl";
      };
    };

    layout = {
      background-color = "#ffffff";
    };

    gestures = {
      hot-corners.off = [];
    };

    cursor = {
      xcursor-theme = "Yaru";
      xcursor-size = 8;
    };

    spawn-sh-at-startup = [
      "eww open-many draw files time help storage sync wifi credits --arg screen_width=$(${getWidthBash}) --arg screen_height=$(${getHeightBash}) --arg scale=$(${getScale}) --restart"
    ];
  };

  prefixVar =
  let
    # Programs that must be available to Niri environment
    envPackages = [
      pkgs.wlr-randr
      pkgs.gawk
      pkgs.coreutils
      pkgs.procps

      pkgs.alacritty
      pkgs.eww-wrapped
      pkgs.wvkbd
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
      "${lib.makeBinPath envPackages}"
    ]
  ]);
}
