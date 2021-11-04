#pragma once

#include <persist/fs/fs.hpp>

#include <whirl/node/time/time_service.hpp>

#include <matrix/server/runtime/detail/disk.hpp>
#include <matrix/fs/fs.hpp>

namespace whirl::matrix {

class FS : public persist::fs::IFileSystem {
 public:
  FS(matrix::fs::FileSystem* impl, node::time::ITimeService* time_service)
      : disk_(time_service), impl_(impl) {
  }

  wheels::Result<bool> Create(const persist::fs::Path& file_path) override {
    return impl_->Create(file_path);
  }

  wheels::Status Unlink(const persist::fs::Path& file_path) override {
    return impl_->Unlink(file_path);
  }

  wheels::Status Truncate(const persist::fs::Path& file_path, size_t new_length) override {
    return impl_->Truncate(file_path, new_length);
  }

  bool Exists(const persist::fs::Path& file_path) const override {
    return impl_->Exists(file_path);
  }

  persist::fs::FileList ListFiles(std::string_view prefix) override {
    // All allocations are made in "userspace"
    persist::fs::FileList listed;

    auto iter = impl_->ListAllFiles();
    while (iter.IsValid()) {
      if ((*iter).starts_with(prefix)) {
        listed.push_back(MakePath(*iter));
      }
      ++iter;
    }

    return listed;
  }

  // FileMode::Append creates file if it does not exist
  wheels::Result<persist::fs::Fd> Open(const persist::fs::Path& file_path,
                                    persist::fs::FileMode mode) override {
    return impl_->Open(file_path, mode);
  }

  // Only for FileMode::Append
  wheels::Status Append(persist::fs::Fd fd, wheels::ConstMemView data) override {
    disk_.Write(data.Size());
    return impl_->Append(fd, data);
  }

  // Only for FileMode::Read
  wheels::Result<size_t> Read(persist::fs::Fd fd,
                              wheels::MutableMemView buffer) override {
    disk_.Read(buffer.Size());  // Blocks
    return impl_->Read(fd, buffer);
  }

  wheels::Status Sync(persist::fs::Fd fd) override {
    return impl_->Sync(fd);
  }

  wheels::Status Close(persist::fs::Fd fd) override {
    return impl_->Close(fd);
  }

  // Paths

  persist::fs::Path MakePath(std::string_view repr) const override {
    return {this, std::string{repr}};
  }

  persist::fs::Path RootPath() const override {
    return {this, std::string(impl_->RootPath())};
  }

  persist::fs::Path TmpPath() const override {
    return {this, std::string(impl_->TmpPath())};
  }

  std::string PathAppend(const std::string& base,
                         const std::string& name) const override {
    return impl_->PathAppend(base, name);
  }

  std::string_view GetNameComponent(const std::string& path) const override {
    return impl_->GetNameComponent(path);
  }

 private:
  // Emulate latency
  matrix::detail::Disk disk_;

  matrix::fs::FileSystem* impl_;
};

}  // namespace whirl::matrix
