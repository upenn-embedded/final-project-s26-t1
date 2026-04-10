{lib, ...}:
let
  margins = 0.05;
  unit = "(min(screen_width, screen_height)*(1 - ${toString margins} * 2) / scale)";
  top = "((screen_height/scale - ${unit})/2)";
  left = "((screen_width/scale - ${unit})/2)";
  button_gap = "(${unit} * 0.01)";
  button_size = "((${unit} - ${button_gap} * 3)/4)";
  buttons_height = "(${button_size} * 2 + ${button_gap})";
  row1_top = "(${top} + (${unit} - ${buttons_height}) / 2)";
  row2_top = "(${row1_top} + ${button_size} + ${button_gap})";
  col2_left = "(${left} + ${button_size} + ${button_gap})";
  col3_left = "(${left} + ${button_size} * 2 + ${button_gap} * 2)";
  col4_left = "(${left} + ${button_size} * 3 + ${button_gap} * 3)";
  title_height = "(${unit}/5)";
  title_top = "(${top} + (${row1_top} - ${top} - ${title_height})/2)";

  mkWindow = { name, x, y, width, height, content }: ''
    (defwindow ${name} [screen_width screen_height scale cmd_alacritty cmd_etch]
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
      command = "etch &";
    }
    {
      name = "time";
      command = "etch &";
    }
    {
      name = "help";
      command = "etch &";
    }
    {
      name = "storage";
      command = "etch &";
    }
    {
      name = "sync";
      command = "etch &";
    }
    {
      name = "wifi";
      command = "\${cmd_alacritty} -e 'nmtui' &";
    }
    {
      name = "credits";
      command = "etch &";
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
      out = acc.out + mkWindow {
        name = elem.name;
        x =
          if lib.trivial.mod acc.i 4 == 0 then left
          else if lib.trivial.mod acc.i 4 == 1 then col2_left
          else if lib.trivial.mod acc.i 4 == 2 then col3_left
          else col4_left;
        y = if acc.i < 4 then row1_top else row2_top;
        width = button_size;
        height = button_size;
        content = if elem ? image then ''
          (overlay
            (button
              :onclick "${elem.command}")
            (transform
              :scale-x { ${button_size} }
              :scale-y { ${button_size} }
              (image
                :image-width { 500 }
                :path "${elem.image}")))
        '' else ''
          (button
            :onclick "${elem.command}"
              "${elem.name}")
        '';
      };
    }) {i = 0; out = "";} buttons).out}
  '';

  style.content = ''
    * {
      all: unset;
    }

    button {
      color: black;
      border: 1mm solid black;
    }
  '';
}
