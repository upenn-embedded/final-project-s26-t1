{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-25.11";
    wrappers = {
      url = "github:BirdeeHub/nix-wrapper-modules";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, wrappers, ... }@inputs:
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
      specialArgs = { inherit inputs; inherit self; };
      modules = [
        ./rpi-install/rpi4.nix
        ./rpi-install/configuration.nix
      ];
    };

    packages = nixpkgs.lib.recursiveUpdate
      (forAllSystems (system: {
        niri = wrappers.wrappers.niri.wrap ({...}: {
          pkgs = import nixpkgs { inherit system; };
          imports = [ ./packages/niri.nix ];
        });
      }))
      {
        aarch64-linux.rpi4B-iso = self.nixosConfigurations.etch-a-sketch.config.system.build.sdImage;
      };

    overlays.default = final: prev: {
      niri = self.packages.${prev.system}.niri;
    };
  };
}
