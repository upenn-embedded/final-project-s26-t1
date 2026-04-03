{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-25.11";
  };

  outputs = { self, nixpkgs, ... }@inputs:
  let
    pkgs = import nixpkgs {
      system = "x86_64-linux";
    };
    pkgs-rpi = import nixpkgs {
      system = "aarch64-linux";
    };
  in
  {
    devShells.x86_64-linux.default = pkgs.mkShell {
      buildInputs = [
        pkgs.jekyll
        pkgs.bundler
      ];
    };

    nixosConfigurations.etch-a-sketch = nixpkgs.lib.nixosSystem {
      system = "aarch64-linux";
      modules = [
        "${nixpkgs}/nixos/modules/installer/sd-card/sd-image-aarch64.nix" # goofy
        ./rpi-install/configuration.nix
      ];
    };

    packages.aarch64-linux.rpi4B-iso = self.nixosConfigurations.etch-a-sketch.config.system.build.sdImage;
  };
}
