{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-25.11";
  };

  outputs = { self, nixpkgs, ... }@inputs:
  let
    forAllSystems = with nixpkgs.lib; genAttrs [
      "x86_64-linux"
      "aarch64-linux"
    ];
  in
  {
    devShells = forAllSystems (system: {
      default = let pkgs = import nixpkgs { inherit system; }; in pkgs.mkShell {
        buildInputs = [
          pkgs.jekyll
          pkgs.bundler
        ];
      };
    });

    nixosConfigurations.etch-a-sketch = nixpkgs.lib.nixosSystem {
      system = "aarch64-linux";
      specialArgs = { inherit inputs; };
      modules = [
        ./rpi-install/rpi4.nix
        ./rpi-install/configuration.nix
      ];
    };

    packages.aarch64-linux.rpi4B-iso = self.nixosConfigurations.etch-a-sketch.config.system.build.sdImage;
  };
}
