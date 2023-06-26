
## Changelog

#### [Upcoming](https://github.com/kuvaus/LlamaGPTJ-chat/compare/v0.3.0...HEAD)


#### [v0.3.0](https://github.com/kuvaus/LlamaGPTJ-chat/releases/tag/v0.3.0)

> 26 June 2023

- Add this [changelog](https://github.com/kuvaus/LlamaGPTJ-chat/blob/main/CHANGELOG.md) :)
- Add sha256 hashes on release so you can verify the binaries
- All binaries are automatically generated with Github actions
- Add signal handling for SIGHUP (macOS, Linux) and CTRL_CLOSE_EVENT (Windows) to fix issue [`#16`](https://github.com/kuvaus/LlamaGPTJ-chat/issues/16)
- This allows you to run chat as a subprocess. The chat subprocess now quits properly if parent app is closed.
- Version information
- Fix segfault on`/help`

#### [v0.2.9](https://github.com/kuvaus/LlamaGPTJ-chat/releases/tag/v0.2.9)

> 22 June 2023

- [Pull request](https://github.com/kuvaus/LlamaGPTJ-chat/pull/18) from [@154pinkchairs](https://github.com/154pinkchairs/) merged. Thanks. :)
- The pull request [`#18`](https://github.com/kuvaus/LlamaGPTJ-chat/pull/18) has the two fixes below:
- Properly handle file paths including tildes [`18e9f36`](https://github.com/kuvaus/LlamaGPTJ-chat/commit/18e9f36)
- Handle buffer allocation errors [`6800dfb`](https://github.com/kuvaus/LlamaGPTJ-chat/commit/6800dfb)
- Better debug mode compilation. May fix issue [`#9`](https://github.com/kuvaus/LlamaGPTJ-chat/issues/9)

#### [v0.2.8](https://github.com/kuvaus/LlamaGPTJ-chat/releases/tag/v0.2.8)
> 16 June 2023

- Adds `--save_dir` option so you can change save directory location
- Default location is `./saves` on the same directory as the chat binary
- See issue [`#13`](https://github.com/kuvaus/LlamaGPTJ-chat/issues/13) for more details

#### [v0.2.7](https://github.com/kuvaus/LlamaGPTJ-chat/releases/tag/v0.2.7)
> 15 June 2023

- Fixes for old macOS.
- Use `-DOLD_MACOS=ON` option when compiling with CMake.
- Tested to compile on High Sierra and Xcode 10

#### [v0.2.6](https://github.com/kuvaus/LlamaGPTJ-chat/releases/tag/v0.2.6)
> 14 June 2023

- You can name saves with `./save NAME` and `./load NAME`
- You can toggle saving and loading off with `--no-saves` flag

#### [v0.2.5](https://github.com/kuvaus/LlamaGPTJ-chat/releases/tag/v0.2.5)
> 13 June 2023

- Save/load state with `./save` and `./load`
- Reset context with `./reset`, help with `./help`
- Makes a `./saves` folder
- Note that a single save can take up to 2Gb
- You can wrap the AI response with tokens using `--b_token` and `--e_token`
- See issue [`#12`](https://github.com/kuvaus/LlamaGPTJ-chat/issues/12) for more details

#### [v0.2.4](https://github.com/kuvaus/LlamaGPTJ-chat/releases/tag/v0.2.4)
> 5 June 2023

- Fix when using json to specify names for logfiles. Fixes issue [`#11`](https://github.com/kuvaus/LlamaGPTJ-chat/issues/11)

#### [v0.2.3](https://github.com/kuvaus/LlamaGPTJ-chat/releases/tag/v0.2.3)
> 4 June 2023

- Fix said ability to reset context... :)

#### [v0.2.2](https://github.com/kuvaus/LlamaGPTJ-chat/releases/tag/v0.2.2)
> 3 June 2023

- Ability to reset context

#### [v0.2.1](https://github.com/kuvaus/LlamaGPTJ-chat/releases/tag/v0.2.1)
> 30 May 2023

- Save and load chat logs
- Use `--save_log` and `--load_log`
- AVX512 option for compilation `-DAVX512=ON`

#### [v0.2.0](https://github.com/kuvaus/LlamaGPTJ-chat/releases/tag/v0.2.0)
> 17 May 2023

- Update gpt4all backend to v0.1.1 [`61a963a`](https://github.com/kuvaus/LlamaGPTJ-chat/commit/61a963a3d220ef157a8504ddde708f33dc2946eb)
- Full Windows Visual Studio compatibility. Finally fixes issue [`#1`](https://github.com/kuvaus/LlamaGPTJ-chat/issues/1)
- Builds from source on aarch64 Linux. Fixes issue [`#3`](https://github.com/kuvaus/LlamaGPTJ-chat/issues/3)
- Full MPT support. Fixes issue [`#4`](https://github.com/kuvaus/LlamaGPTJ-chat/issues/4)

#### v0.1.9
> 16 May 2023

- Code cleaning and reordering
- `llmodel_create_model()` function

#### v0.1.8
> 13 May 2023

- Add support for MPT models
- Uses [gpt4all-backend](https://github.com/nomic-ai/gpt4all)

#### v0.1.7
> 12 May 2023

- First [pull request](https://github.com/kuvaus/LlamaGPTJ-chat/pull/2)  from [@itz-coffee](https://github.com/itz-coffee/) merged. Thanks. :)
- The pull request [`#2`](https://github.com/kuvaus/LlamaGPTJ-chat/pull/2) adds the feature below:
- Add --no-animation flag [`fdc2ac3`](https///github.com/kuvaus/LlamaGPTJ-chat/commit/fdc2ac3)
- Support for old macOS

#### v0.1.6
> 4 May 2023

- Parse parameters from json files
- Use `-j FNAME` or`--load_json  FNAME`

#### v0.1.5
> 3 May 2023

- MinGW compilation on Windows

#### v0.1.4
> 1 May 2023

- v0.1.4 had no tags
- It was part of `cmake-release.yml` rewrite to enable MinGW [`e7e1ebf`](https://github.com/kuvaus/LlamaGPTJ-chat/commit/e7e1ebf97d696d069bbc0ae7f0ed078739fb6642)

#### v0.1.3
> 1 May 2023

- Add loading of prompt template files
- Use `--load_template` for loading
- See `prompt_template_sample.txt` for a sample


#### v0.1.2
> 30 April 2023

- Automatic memory handling for the model

#### v0.1.1
> 29 April 2023

- Windows compilation fixes

#### v0.1.0
> 29 April 2023


- Before this, progress was in  [GPTJ-chat](https://github.com/kuvaus/GPTJ-chat/) and [Llama-chat](https://github.com/kuvaus/Llama-chat/)
- First version
