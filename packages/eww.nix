{lib, ...}:
let
  margins = 0.05;
  unit = "(min(screen_width, screen_height)*(1 - ${toString margins} * 2) / scale)";
  top = "((screen_height/scale - ${unit})/2)";
  left = "((screen_width/scale - ${unit})/2)";
  button_gap = "(${unit} * 0.01)";
  button_size = "((${unit} - ${button_gap} * 3)/4)";
  svg_proportional_line_thickness = 0.2 / 10;
  border_size = "(${button_size}*${builtins.toString svg_proportional_line_thickness})";
  inner_button_size = "(${button_size} - ${border_size}*2)";
  buttons_height = "(${button_size} * 2 + ${button_gap})";
  row1_top = "(${top} + (${unit} - ${buttons_height}) / 2)";
  row2_top = "(${row1_top} + ${button_size} + ${button_gap})";
  col2_left = "(${left} + ${button_size} + ${button_gap})";
  col3_left = "(${left} + ${button_size} * 2 + ${button_gap} * 2)";
  col4_left = "(${left} + ${button_size} * 3 + ${button_gap} * 3)";
  title_height = "(${unit}/5)";
  title_top = "(${top} + (${row1_top} - ${top} - ${title_height})/2)";

  mkWindow = { name, x, y, width, height, content }: ''
    (defwindow ${name}
      [
        screen_width
        screen_height
        scale
        cmd_alacritty
        cmd_browser
        cmd_etch
        cmd_files
      ]
      :monitor 0
      :geometry (geometry
        :x { ${x} }
        :y { ${y} }
        :width { ${width} }
        :height { ${height} })
      :stacking "bg"
      :exclusive "false"
      :focusable "none"
      ${content})
  '';

  mkDebugArea = { name, x, y, width, height, color }: mkWindow {
    inherit name x y width height;
    content = ''(box :style "background-color: ${color};")'';
  };

  buttons = [
    {
      name = "draw";
      command = "\${cmd_etch} &";
      image = ../assets/etch-a-sketch.svg;
    }
    {
      name = "files";
      command = "\${cmd_files} ~ &";
      image = ../assets/files.svg;
    }
    {
      name = "time";
      overlays = [
        ''
          (transform
            :translate-x { ${border_size} }
            :translate-y { ${border_size} }
              (transform
                :scale-x { ${inner_button_size} }
                :scale-y { ${inner_button_size} }
                  (image
                    :image-width 500
                    :path "${../assets/clock.svg}")))
        ''
      ];
    }
    {
      name = "battery";
      image = ../assets/battery.svg;
    }
    {
      name = "storage";
      overlays = [
        ''
          (transform
            :translate-x { ${border_size} }
            :translate-y { ${border_size} }
              (transform
                :scale-x { ${inner_button_size} }
                :scale-y { ${inner_button_size} }
                  (circular-progress
                    :value { EWW_DISK["/"].used_perc }
                    :start-at 40
                    :thickness { ${border_size}*4 }
                    :clockwise true
                    :style "color: black; background-color: #dddddd;")))
        ''
      ];
    }
    {
      name = "sync";
      image = ../assets/sync.svg;
    }
    {
      name = "wifi";
      command = "\${cmd_alacritty} -e 'nmtui' &";
      image = ../assets/wifi.svg;
    }
    {
      name = "browser";
      command = "\${cmd_browser} https://garticphone.com &";
      image = ../assets/earth.svg;
    }
  ];
in
{
  yuck = ''
    ${mkDebugArea {
      name = "debug_unit";
      x = left;
      y = top;
      width = unit;
      height = unit;
      color = "green";
    }}

    ${mkDebugArea {
      name = "debug_button_area";
      x = left;
      y = row1_top;
      width = unit;
      height = buttons_height;
      color = "blue";
    }}

    ${mkDebugArea {
      name = "debug_title_area";
      x = left;
      y = title_top;
      width = unit;
      height = title_height;
      color = "red";
    }}

    ${(builtins.foldl' (acc: elem: {
      i = acc.i + 1;
      out = acc.out + mkWindow
      (let
        overlays = (if elem ? image then [''
          (transform
            :translate-x { ${border_size} }
            :translate-y { ${border_size} }
              (transform
                :scale-x { ${inner_button_size} }
                :scale-y { ${inner_button_size} }
                (image
                  :image-width { 500 }
                  :path "${elem.image}")))
        ''] else [])
        ++ (elem.overlays or []);
      in
      {
        name = elem.name;
        x =
          if lib.trivial.mod acc.i 4 == 0 then left
          else if lib.trivial.mod acc.i 4 == 1 then col2_left
          else if lib.trivial.mod acc.i 4 == 2 then col3_left
          else col4_left;
        y = if acc.i < 4 then row1_top else row2_top;
        width = button_size;
        height = button_size;
        content = ''
          (overlay
            (button
              ${if elem ? command then '':onclick "${elem.command}"'' else ""}
              :style "color: black; border: ''${${border_size}}px solid black;";
                ${if builtins.length overlays > 0 then "" else ''"${elem.name}"''})
            ${builtins.concatStringsSep " " overlays})
        '';
      });
    }) {i = 0; out = "";} buttons).out}
  '';

  style.content = ''
    * {
      all: unset;
    }
  '';
}
