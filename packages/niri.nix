# Nix Settings Format: https://birdeehub.github.io/nix-wrapper-modules/wrapperModules/niri.html
# Niri Config Wiki: https://github.com/niri-wm/niri/wiki/Getting-Started
{pkgs, lib, config, ...}:
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
        spawn-sh = ''
          if niri msg -j focused-window | grep -q '"id":'; then
            niri msg action close-window
          else
            etch &
          fi
        '';
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

    spawn-sh-at-startup =
    let
      eww_windows = [
        "title" # Needs to be below everything

        "draw"
        "files"
        "time"
        "battery"
        "storage"
        "sync"
        "wifi"
        "browser"
      ];
      eww_args = {
        screen_width = "$(${getWidthBash})";
        screen_height = "$(${getHeightBash})";
        scale = "$(${getScale})";
        cmd_alacritty = "alacritty";
        cmd_browser = "firefox";
        cmd_etch = "etch";
        cmd_files = "pcmanfm-qt";
      };
    in
    [
      "pkill -x eww; eww daemon && eww open-many ${builtins.concatStringsSep " " eww_windows} ${lib.foldlAttrs
        (acc: arg: cmd: acc + ''--arg ${arg}=${cmd} '')
        "" eww_args
      }"
    ];
    #outputs."DSI-1" = {
    #  transform = "180";
    #};
  };

  prefixVar =
  let
    # Programs that must be available to Niri environment
    envPackages = [
      "${placeholder config.outputName}" # self
      pkgs.wlr-randr
      pkgs.gawk
      pkgs.coreutils
      pkgs.procps

      pkgs.alacritty
      pkgs.etch
      pkgs.eww-wrapped
      pkgs.firefox
      pkgs.pcmanfm-qt
      pkgs.wvkbd
    ]; 
  in
  ([
    # Make cursor theme discoverable
    [
      "XCURSOR_PATH"
      ":"
      "${pkgs.etch-a-sketch-Yaru-theme}/share/icons"
    ]
    [
      "PATH"
      ":"
      "${lib.makeBinPath envPackages}"
    ]
  ]);
}
