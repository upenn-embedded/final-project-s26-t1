{
  yaru-theme,
  win2xcur,
  lib,
  ...
}:
yaru-theme.overrideAttrs (prev: {
#  name = "etch-a-sketch-Yaru-theme";
  postInstall = (prev.postInstall or "") + ''
    cp ${../assets/left_ptr.cur} $out/share/icons/Yaru/cursors/left_ptr.cur
    cp ${../assets/left_ptr.cur} $out/share/icons/Yaru/cursors/default.cur
    cp ${../assets/left_ptr.cur} $out/share/icons/Yaru/cursors/arrow.cur

    ${lib.getExe' win2xcur "win2xcur"} $out/share/icons/Yaru/cursors/left_ptr.cur -o $out/share/icons/Yaru/cursors

    cp $out/share/icons/Yaru/cursors/left_ptr $out/share/icons/Yaru/cursors/default
    cp $out/share/icons/Yaru/cursors/left_ptr $out/share/icons/Yaru/cursors/arrow
  '';
})
