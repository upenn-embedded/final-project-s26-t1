{
  rustPlatform,
  my-rust,
  ...
}:
rustPlatform.buildRustPackage.override {
  rustc = my-rust;
} {
  pname = "etch";
  version = "0.0.2";
  src = ./.;

  buildInputs = [
  ];

  nativeBuildInputs = [
  ];

  cargoLock.lockFile = ./Cargo.lock;
}
