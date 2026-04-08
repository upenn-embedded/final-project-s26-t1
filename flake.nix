{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-25.11";
    wrappers = {
      url = "github:BirdeeHub/nix-wrapper-modules/main";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    nix-hardware.url = "github:nixos/nixos-hardware/master";
    # virtual keyboard support in niri is temporarily removed upstream https://github.com/niri-wm/niri/issues/403#issuecomment-4129099807
    niri = {
      url = "github:niri-wm/niri?rev=74d14be01f606ff519f1c4433715785c00aa0caa";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    # wvkbd in stable does not work
    nixpkgs-unstable.url = "github:nixos/nixpkgs?ref=nixos-unstable";
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

    overlays.default = final: prev: let system = prev.stdenv.hostPlatform.system; in {
      niri-wrapped = self.packages.${system}.niri-wrapped;
      eww-wrapped = self.packages.${system}.eww-wrapped;
      niri = inputs.niri.packages.${system}.default;
      wvkbd = inputs.nixpkgs-unstable.legacyPackages.${system}.wvkbd;
    };
  };
}
