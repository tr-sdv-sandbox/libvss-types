{
  description = "SDK-X C++ library flake";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs";

  outputs = { self, nixpkgs, ... }:
    let
      system = "aarch64-darwin"; # Change if needed
      pkgs = import nixpkgs { inherit system; };
    in {
      packages.${system}.default = pkgs.stdenv.mkDerivation {
        pname = "vss-types";
        version = "1.0.0";
        src = ./.;

        nativeBuildInputs = [
          pkgs.cmake
        ];

        buildInputs = [
          pkgs.gcc
          pkgs.protobuf
          pkgs.glog
          pkgs.yaml-cpp
          pkgs.re2
          pkgs.c-ares
          pkgs.gtest
          pkgs.grpc
          pkgs.zlib
          pkgs.openssl
          pkgs.nlohmann_json
        ];

        cmakeFlags = [
          "-DCMAKE_BUILD_TYPE=Release"
          "-DBUILD_EXAMPLES=OFF"
          "-DBUILD_TESTS=OFF"
        ];

        # Use CMake's install directive
        installPhase = ''
          cmake --install . --prefix $out
        '';

        meta = {
          description = "SDK-X C++ library";
          license = pkgs.lib.licenses.mit;
          platforms = pkgs.lib.platforms.unix;
        };
      };
    };
}
