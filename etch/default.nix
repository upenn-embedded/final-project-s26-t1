{
  lib,
  libxkbcommon,
  makeWrapper,
  my-rust,
  rustPlatform,
  vulkan-loader,
  wayland,
  ...
}:
let libPath = lib.makeLibraryPath [
  libxkbcommon
  vulkan-loader
  wayland
];
in
rustPlatform.buildRustPackage.override {
  rustc = my-rust;
} {
  pname = "etch";
  version = "0.0.2";
  src = ./.;

  buildInputs = [
  ];

  nativeBuildInputs = [
    makeWrapper
  ];

  cargoLock.lockFile = ./Cargo.lock;

  postInstall = ''
    wrapProgram "$out/bin/etch" --prefix LD_LIBRARY_PATH : "${libPath}"
  '';

  doCheck = false;
}
