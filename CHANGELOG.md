# changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog] and this project attempts to adhere to [Semantic Versioning].

Changes will be documented under Added, Changed, Deprecated, Removed, Fixed or Security headers.

## Known bugs

## Unreleased

### Pending

## [1.3.0] - 2026-08-10
### changed
- Adds previous/next controller to awake/sleep. Breaks compatibility.

## [1.2.1] - 2026-05-23
### fixed
- Fixes loop iteration data definitions, timestep should be measured in doubles and not floats.

## [1.2.0] - 2025-06-09
### added
- Adds input recorders and replayers.

## [1.1.2] - 2025-04-13
### added
- Adds set_control_data to config base, so it can use the internal json allocator instead of dabbling into json stuff directly.

## [1.1.1] - 2024-06-29
### added
- Added the id_to_path_mapper class.
- Added passthrough audio methods.
### changed
- Changes defaults to audio methods.

## [1.1.0] - 2024-03-06
### added
- Promoted input parsing functions from the dfw-jumpstart to this library and changes how they behave.
## [1.1.0] - 2024-03-06
### added
- Promoted input parsing functions from the dfw-jumpstart to this library and changes how they behave.

### changed
- Changes how input configuration behaves (many inputs for a command are now the rule).
- Changes how inputs are reflected in the config file.
- Both are BACKWARDS BREAKING changes!.

## [1.0.4] - 2024-02-08
### changed
- changes build system

## [1.0.3] - 2023-07-29
### changed
- Can't remember now.

## [1.0.2] - 2022-08-24
### changed
- Changes meaning of state driver interface init().

### added
- Adds state driver interface start(), which was the previous init()

## [1.0.1] - 2021-7-20
### fixed
- Fixes compatibility with log module.

## [1.0.0] - 2020-5-15
### added

- adds a stack of controllers to the state controller.

### changed

- not really the first version, but the first changelog.

### removed

- not really the first version, but the first changelog.
