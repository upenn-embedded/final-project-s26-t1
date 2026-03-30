{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-25.11";
  };

  outputs = { self, ... }@inputs:
  let
    pkgs = import inputs.nixpkgs {
      system = "x86_64-linux";
    };
  in
  {
    devShells.x86_64-linux.default = pkgs.mkShell {
      buildInputs = [
        pkgs.jekyll
        pkgs.bundler
      ];
    };
  };
}
