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
  cargoLock.outputHashes = {
    "ecolor-0.34.1" = "sha256-1T3Hwr8eBICVWm5HGQdxXtOdC6/5Wq0iPx4GPlWOSes=";
    "naga-29.0.1" = "sha256-MlEx5KhtO/8zLYbxndbsSPmyYt2GPlPoioRne39o9Zw=";
  };

  postInstall = ''
    wrapProgram "$out/bin/etch" --prefix LD_LIBRARY_PATH : "${libPath}"
  '';

  doCheck = false;
}
