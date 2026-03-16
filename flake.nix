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
        pkgs.glslang
        
        # Vulkan:
        pkgs.vulkan-headers
        pkgs.vk-bootstrap
        pkgs.vulkan-memory-allocator
        
        # SDL:
        pkgs.sdl3.dev

        pkgs.imgui
        
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
      VULKAN_LIBRARY = "${pkgs.vulkan-loader.out}/lib/libvulkan.so";

      # find_package can use this var to find libraries.
      CMAKE_PREFIX_PATH = "${pkgs.vulkan-loader.out};${pkgs.imgui.out}";

      shellHook = ''
        echo "Entering shell!"
      '';
    };

    debug = pkgs.stdenv.mkDerivation (finalAttrs: {
      pname = "vulkan-app";
      version = "debug";
      src = ./.;

      dontStrip = true;

      cmakeFlags = [
        "-DCMAKE_BUILD_TYPE=Debug"
      ];

      nativeBuildInputs = [ 
        pkgs.cmake
        pkgs.ninja
        pkgs.makeWrapper
        
        pkgs.imgui

        # Vulkan:
        pkgs.vulkan-headers
        pkgs.vk-bootstrap
        pkgs.vulkan-memory-allocator
      ];
      buildInputs = [
        pkgs.vulkan-loader
        pkgs.sdl3
        pkgs.mesa
        pkgs.imgui
        pkgs.vulkan-memory-allocator
      ];
      
      installPhase = ''
        runHook preInstall
        mkdir -p $out/bin/shaders
        cp vulkan-app $out/bin
        cp -r $src/src/shaders/*.spv $out/bin/shaders
        runHook postInstall
      '';

      postFixup = 
      let
        libs = with pkgs; [
          libGL.out
          mesa
        ];
      in
      ''
        wrapProgram $out/bin/vulkan-app \
          --set LD_LIBRARY_PATH ${pkgs.lib.makeLibraryPath libs} \
          --set SHADER_PATH shaders
      '';
    });
  };
}
