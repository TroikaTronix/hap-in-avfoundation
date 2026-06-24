# hap-in-avfoundation

This repository is required to build Isadora.

This TroikaTronix fork is not in close synchronization with the upstream master. It contains local build-system and compatibility changes needed by the Isadora build process, while also missing some changes now present upstream.

## TroikaTronix Changes

The TroikaTronix fork appears to have been adjusted primarily so that the library and its dependencies can be built reliably as part of Isadora.

Notable local changes include:

- Added shared Xcode schemes for `AVFtoHap` and `HapInAVFoundation-static`.
- Added shared Xcode schemes for the external `snappy` and `squish` projects, including static-library schemes.
- Added `.gitignore` entries for local build products and local xcconfig files:
  - `hap-in-avfoundation-debug.xcconfig`
  - `hap-in-avfoundation-release.xcconfig`
  - `build`
- Removed hard-coded upstream development-team settings.
- Changed signing configuration to manual signing, with an empty macOS development team setting.
- Set the main HapInAVFoundation target deployment target to macOS 10.12.
- Set the external `snappy` project deployment target to macOS 10.12.
- Set the external `squish` project deployment target to macOS 10.12.
- Adjusted the Xcode project object version from 54 to 53, presumably for compatibility with the version of Xcode used in the Isadora build system.
- Removed `Sparkle.framework` from one of the framework build phases.
- Removed the explicit `productRefGroup` reference from the Xcode project.
- Added conditional compilation around Hap7 / BC7-related code using `CAN_COMPILE_HAP7`.
- Added `CAN_COMPILE_HAP7` to `HapCodecSubTypes.h` so Hap7 support is only compiled when the SDK and deployment target support macOS 10.15 or later.
- Wrapped uses of `kHap7AlphaCodecSubType` and the ATE BC7 encoder so the project can still compile when targeting macOS versions earlier than 10.15.

## Changes Present Upstream but Not Fully Reflected Here

Compared to the current upstream master, this fork appears to be missing or intentionally disabling some newer upstream behavior:

- The upstream project includes Hap7 / BC7 support more directly. In this fork, Hap7 support is guarded by `CAN_COMPILE_HAP7` and is disabled when building for deployment targets earlier than macOS 10.15.
- The upstream project appears to target macOS 10.13 or later in several places, while the TroikaTronix fork lowers those targets to macOS 10.12.
- The upstream project includes a hard-coded development team in several build settings; the TroikaTronix fork removes or clears those settings.
- The upstream project includes signing and project settings that are not suitable for the TroikaTronix build process, so they have been changed locally.
- The upstream project includes a Sparkle framework linkage in one build phase that has been removed in this fork.
- Upstream project metadata appears to have been saved with a newer Xcode project object version.

## Notes

Because this repository is required for the Isadora build, changes from upstream should be merged carefully. In particular, pay attention to:

- macOS deployment target changes.
- Hap7 / BC7 support and availability checks.
- static-library build schemes required by the Isadora build.
- local signing assumptions.
- external dependency schemes for `snappy` and `squish`.

This fork should be treated as an Isadora build dependency rather than a clean mirror of upstream.
