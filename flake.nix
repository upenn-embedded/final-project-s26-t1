{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-25.11";
    wrappers = {
      url = "github:BirdeeHub/nix-wrapper-modules/main";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    nix-hardware.url = "github:nixos/nixos-hardware/master";
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
        niri-wrapped = wrappers.wrappers.niri.wrap ({...}: {
          pkgs = import nixpkgs {
            inherit system;
            overlays = [ self.overlays.default ];
          };
          imports = [ ./packages/niri.nix ];
        });
        eww-wrapped = wrappers.wrappers.eww.wrap ({...}: {
          pkgs = import nixpkgs {
            inherit system;
            overlays = [ self.overlays.default ];
          };
          imports = [ ./packages/eww.nix ];
        });
      }))
      {
        aarch64-linux.rpi4B-img = self.nixosConfigurations.etch-a-sketch.config.system.build.sdImage;
      };

    overlays.default = final: prev: {
      niri-wrapped = self.packages.${prev.stdenv.hostPlatform.system}.niri-wrapped;
      eww-wrapped = self.packages.${prev.stdenv.hostPlatform.system}.eww-wrapped;
    };
  };
}
