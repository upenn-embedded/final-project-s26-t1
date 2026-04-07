{...}:
{
  yuck = ''
    (defwindow draw
      :monitor 0
      :geometry (geometry
        :x "0%"
        :y "0%"
        :width "80%"
        :height "80%"
        :anchor "center")
      :stacking "bg"
      :exclusive "false"
      :focusable "none"

      (button
        :onclick "alacritty -e 'nmtui' &"
        "WiFi"
      )
    )
  '';

  style = ''
    * {
      all: unset;
    }

    button {
      background-color: #ddddff;
      color: black;
    }
  '';
}
