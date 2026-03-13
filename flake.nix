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
        pkgs.pkg-config
        
        # Vulkan:
        pkgs.vulkan-headers
        pkgs.vk-bootstrap
        pkgs.vulkan-memory-allocator
        
        # SDL:
        pkgs.sdl3.dev
        
        # Debugging tools:
        pkgs.gef
        pkgs.strace
      ];
      
      # Set environment variables:
      LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath [
        #pkgs.xorg.libX11
        #pkgs.xorg.libXcursor
        pkgs.mesa
        pkgs.vulkan-loader
        pkgs.sdl3
      ];
      VULKAN_SDK = "${pkgs.vulkan-headers}";

      shellHook = ''
        echo "Entering shell!"
      '';
    };

    debug = pkgs.stdenv.mkDerivation (finalAttrs: {
      pname = "build";
      version = "debug";
      src = ./.;

      dontStrip = true;

      cmakeFlags = [
        "-DCMAKE_BUILD_TYPE=Debug"
      ];

      nativeBuildInputs = [ 
        pkgs.cmake
        pkgs.ninja
        #pkgs.makeWrapper

        # Vulkan:
        pkgs.vulkan-headers
        pkgs.vk-bootstrap
        pkgs.vulkan-memory-allocator
      ];
      buildInputs = [
        pkgs.vulkan-loader
        pkgs.sdl3
        pkgs.mesa
      ];
    });
  };
}
