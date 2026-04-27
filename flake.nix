{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-25.11";
    wrappers = {
      url = "github:BirdeeHub/nix-wrapper-modules/main";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    nix-hardware.url = "github:nixos/nixos-hardware/master";
    rust-overlay.url = "github:oxalica/rust-overlay";
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
    get-rust-bin = pkgs: pkgs.rust-bin.stable.latest.default;
    pkgsBySystem = system: import nixpkgs {
      inherit system;
      overlays = [
        self.overlays.default
        (import inputs.rust-overlay)
        (final: _: {
          my-rust = get-rust-bin final;
        })
      ];
    };
  in
  {
    devShells = forAllSystems (system: {
      default =
      let
        pkgs = pkgsBySystem system;
        ld_libraries = [
          pkgs.wayland
          pkgs.libxkbcommon
          pkgs.vulkan-loader
        ];
      in pkgs.mkShell {
        buildInputs = [
          pkgs.bundler
          (get-rust-bin pkgs)
        ];

        LD_LIBRARY_PATH="${pkgs.lib.makeLibraryPath ld_libraries}:$LD_LIBRARY_PATH";
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
      (forAllSystems (system: let pkgs = pkgsBySystem system; in {
        niri-wrapped = wrappers.wrappers.niri.wrap ({...}: {
          inherit pkgs;
          imports = [ ./packages/niri.nix ];
        });
        eww-wrapped = wrappers.wrappers.eww.wrap ({...}: {
          inherit pkgs;
          imports = [ ./packages/eww.nix ];
        });
        etch = pkgs.callPackage ./etch/default.nix {};
        etch-a-sketch-Yaru-theme = pkgs.callPackage ./packages/etch-a-sketch-Yaru-theme.nix {};
      }))
      {
        aarch64-linux.rpi4B-img = self.nixosConfigurations.etch-a-sketch.config.system.build.sdImage;
      };

    overlays.default = final: prev: let system = prev.stdenv.hostPlatform.system; in {
      niri-wrapped = self.packages.${system}.niri-wrapped;
      etch = self.packages.${system}.etch;
      etch-a-sketch-Yaru-theme = self.packages.${system}.etch-a-sketch-Yaru-theme;
      eww-wrapped = self.packages.${system}.eww-wrapped;
      niri = inputs.niri.packages.${system}.default;
      wvkbd = inputs.nixpkgs-unstable.legacyPackages.${system}.wvkbd;
    };
  };
}
