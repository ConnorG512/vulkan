{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }: 
  let
    pkgs = nixpkgs.legacyPackages.x86_64-linux;
  in 
  {
    devShells.x86_64-linux.default = pkgs.mkShell {
      packages = [ 
        pkgs.clang-tools
        pkgs.cmake
        pkgs.ninja
        
        # Vulkan:
        pkgs.vulkan-headers
        pkgs.vk-bootstrap
        pkgs.vulkan-memory-allocator
      ];

      shellHook = ''
        echo "Entering shell!"
      '';
    };

    build = pkgs.stdenv.mkDerivation (finalAttrs: {
      pname = "build";
      version = "debug";
      src = ./.;

      dontStrip = true;

      nativeBuildInputs = [ 
        pkgs.cmake
        pkgs.ninja
        pkgs.vulkan-headers
        pkgs.vk-bootstrap
        pkgs.vulkan-memory-allocator
      ];
      buildInputs = [
        pkgs.vulkan-loader
      ];
    });
  };
}
