{pkgs, lib, ...}:
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
in
{
  yuck = ''
    (defwindow debug_unit [screen_width screen_height scale]
      :monitor 0
      :geometry (geometry
        :x { ${left} }
        :y { ${top} }
        :width { ${unit} }
        :height { ${unit} })
      :stacking "bg"
      :exclusive "false"
      :focusable "none"
      (box :style "background-color: green;"))

    (defwindow debug_button_area [screen_width screen_height scale]
      :monitor 0
      :geometry (geometry
        :x { ${left} }
        :y { ${row1_top} }
        :width { ${unit} }
        :height { ${buttons_height} })
      :stacking "bg"
      :exclusive "false"
      :focusable "none"
      (box :style "background-color: blue;"))

    (defwindow debug_title_area [screen_width screen_height scale]
      :monitor 0
      :geometry (geometry
        :x { ${left} }
        :y { ${title_top} }
        :width { ${unit} }
        :height { ${title_height} })
      :stacking "bg"
      :exclusive "false"
      :focusable "none"
        (box :style "background-color: red;"))

    (defwindow draw [screen_width screen_height scale]
      :monitor 0
      :geometry (geometry
        :x { ${left} }
        :y { ${row1_top} }
        :width { ${button_size} }
        :height { ${button_size} })
      :stacking "bg"
      :exclusive "false"
      :focusable "none"
        (button
          :onclick "alacritty -e 'nmtui' &"
            (image
              :path "${../assets/etch-a-sketch.svg}")))

    (defwindow files [screen_width screen_height scale]
      :monitor 0
      :geometry (geometry
        :x { ${col2_left} }
        :y { ${row1_top} }
        :width { ${button_size} }
        :height { ${button_size} })
      :stacking "bg"
      :exclusive "false"
      :focusable "none"
        (button
          :onclick "alacritty -e 'nmtui' &"
            "Files"))

    (defwindow time [screen_width screen_height scale]
      :monitor 0
      :geometry (geometry
        :x { ${col3_left} }
        :y { ${row1_top} }
        :width { ${button_size} }
        :height { ${button_size} })
      :stacking "bg"
      :exclusive "false"
      :focusable "none"
        (button
          :onclick "alacritty -e 'nmtui' &"
            "time"))

    (defwindow help [screen_width screen_height scale]
      :monitor 0
      :geometry (geometry
        :x { ${col4_left} }
        :y { ${row1_top} }
        :width { ${button_size} }
        :height { ${button_size} })
      :stacking "bg"
      :exclusive "false"
      :focusable "none"
        (button
          :onclick "alacritty -e 'nmtui' &"
            "help"))

    (defwindow storage [screen_width screen_height scale]
      :monitor 0
      :geometry (geometry
        :x { ${left} }
        :y { ${row2_top} }
        :width { ${button_size} }
        :height { ${button_size} })
      :stacking "bg"
      :exclusive "false"
      :focusable "none"
        (button
          :onclick "alacritty -e 'nmtui' &"
            "storage"))

    (defwindow sync [screen_width screen_height scale]
      :monitor 0
      :geometry (geometry
        :x { ${col2_left} }
        :y { ${row2_top} }
        :width { ${button_size} }
        :height { ${button_size} })
      :stacking "bg"
      :exclusive "false"
      :focusable "none"
        (button
          :onclick "alacritty -e 'nmtui' &"
            "sync"))

    (defwindow wifi [screen_width screen_height scale]
      :monitor 0
      :geometry (geometry
        :x { ${col3_left} }
        :y { ${row2_top} }
        :width { ${button_size} }
        :height { ${button_size} })
      :stacking "bg"
      :exclusive "false"
      :focusable "none"
        (button
          :onclick "alacritty -e 'nmtui' &"
            "wifi"))

    (defwindow credits [screen_width screen_height scale]
      :monitor 0
      :geometry (geometry
        :x { ${col4_left} }
        :y { ${row2_top} }
        :width { ${button_size} }
        :height { ${button_size} })
      :stacking "bg"
      :exclusive "false"
      :focusable "none"
        (button
          :onclick "alacritty -e 'nmtui' &"
            "credits"))
  '';

  style = ''
    * {
      all: unset;
    }

    button {
      background-color: #ddddff;
      color: black;
      border: 0.2mm solid black;
    }
  '';
}
