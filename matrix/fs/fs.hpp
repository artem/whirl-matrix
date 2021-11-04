#pragma once

#include <persist/fs/fs.hpp>

#include <matrix/fs/file.hpp>
#include <matrix/fs/path.hpp>

#include <timber/logger.hpp>

#include <wheels/result/result.hpp>

#include <map>
#include <string>
#include <memory>
#include <vector>

namespace whirl::matrix::fs {

//////////////////////////////////////////////////////////////////////

class FileSystem {
  using FileRef = std::shared_ptr<File>;

  struct OpenedFile {
    persist::fs::Fd fd;
    persist::fs::Path path;
    persist::fs::FileMode mode;
    size_t offset;
    FileRef file;
  };

  using Files = std::map<Path, FileRef>;

  class DirIterator {
   public:
    DirIterator(Files& files) : it_(files.begin()), end_(files.end()) {
    }

    const Path& operator*() {
      return it_->first;
    }

    bool IsValid() const {
      return it_ != end_;
    }

    void operator++() {
      ++it_;
    }

   private:
    Files::const_iterator it_;
    Files::const_iterator end_;
  };

 public:
  FileSystem();

  // System calls
  // Context: Server

  // Metadata

  wheels::Result<bool> Create(const persist::fs::Path& file_path);
  wheels::Status Truncate(const persist::fs::Path& file_path, size_t new_size);
  wheels::Status Unlink(const persist::fs::Path& file_path);
  bool Exists(const persist::fs::Path& file_path) const;

  DirIterator ListAllFiles();

  // Data

  wheels::Result<persist::fs::Fd> Open(const persist::fs::Path& file_path,
                                    persist::fs::FileMode mode);

  wheels::Result<size_t> Read(persist::fs::Fd fd, wheels::MutableMemView buffer);
  wheels::Status Append(persist::fs::Fd fd, wheels::ConstMemView data);
  wheels::Status Sync(persist::fs::Fd fd);
  wheels::Status Close(persist::fs::Fd fd);

  // Paths

  std::string_view RootPath() const;

  std::string_view TmpPath() const;

  std::string PathAppend(const std::string& base_path,
                         const std::string& name) const;

  std::pair<std::string_view, std::string_view> PathSplit(const std::string& path) const;

  // Simulation

  // Fault injection
  void Corrupt(const persist::fs::Path& file_path);

  // On crash
  void Reset();

  size_t ComputeDigest() const;

 private:
  FileRef FindOrCreateFile(const persist::fs::Path& file_path,
                           persist::fs::FileMode open_mode);
  FileRef FindExistingFile(const persist::fs::Path& file_path);

  static FileRef CreateFile();

  size_t InitOffset(FileRef f, persist::fs::FileMode open_mode);

  void CheckMode(OpenedFile& of, persist::fs::FileMode expected);

  OpenedFile& GetOpenedFile(persist::fs::Fd fd);

  [[noreturn]] void RaiseError(const std::string& message);

 private:
  // Persistent state
  Files files_;

  // Process (volatile) state
  std::map<persist::fs::Fd, OpenedFile> opened_files_;
  persist::fs::Fd next_fd_{0};

  timber::Logger logger_;
};

}  // namespace whirl::matrix::fs
