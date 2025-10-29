# SOME/IP Gateway

The SOME/IP Gateway adds SOME/IP network access to S-CORE.
It bridges SOME/IP services from and to IPC and translates user defined types.

The SOME/IP Gateway uses a plugin architecture to realize SOME/IP network access and payload transformation.
The implementation tries to stick as much as possible to the [proposed architecture](https://eclipse-score.github.io/score/main/features/communication/some_ip_gateway/architecture/index.html).
Because the SOME/IP Gateway code shall be kept generic the IPC code was moved into the payload transformation plugin.
Otherwise it was not possible to keep the gateway code service agnostic.

This is the code for [Elektrobits contribution](https://github.com/eclipse-score/score/issues/1830)

## Proof of concept

In its current state the code is a proof of concept to demonstrate how a SOME/IP Gateway can be implemented.
It only shows that reception of SOME/IP events at IPC is possible.
Methods and fields have been omitted because IPC does not support them yet.
Also E2E checks and actual binary payload transformation have been omitted due to time constraints.

---

## 📂 Project Structure

| File/Folder                         | Description                                       |
| ----------------------------------- | ------------------------------------------------- |
| `README.md`                         | Short description & build instructions            |
| `src/`                              | Source files for the module                       |
| `docs/`                             | Documentation (Doxygen for C++ / mdBook for Rust) |
| `.github/workflows/`                | CI/CD pipelines                                   |
| `.vscode/`                          | Recommended VS Code settings                      |
| `.bazelrc`, `MODULE.bazel`, `BUILD` | Bazel configuration & settings                    |
| `project_config.bzl`                | Project-specific metadata for Bazel macros        |
| `LICENSE.md`                        | Licensing information                             |
| `CONTRIBUTION.md`                   | Contribution guidelines                           |

---

## 🚀 Getting Started

### 1️⃣ Clone the Repository

```sh
git clone https://github.com/Elektrobit/inc_gateway.git
cd inc_gateway
```

### 2️⃣ Build the Examples of module

To build all targets of the module the following command can be used:

```sh
bazel build //src/...
```

This command will instruct Bazel to build all targets that are under Bazel
package `src/`.

If you are only interested in the SDK for building a SOME/IP plugin, build the tar archive of the SDK:

```sh
bazel build //src:inc_gateway_sdk
cp bazel-bin/src/inc_gateway_sdk.tar .
```

`inc_gateway_sdk.tar` contains the libraries and headers needed to build a SOME/IP plugin.
It does not provide any build system integration yet.

### 3️⃣ Run Tests

```sh
bazel test //tests/...
```
