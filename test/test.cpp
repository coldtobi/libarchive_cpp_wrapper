/*
 * testsuite for libarchive_cpp_wrapper
 *
 *  Created on: Feb 8, 2021
 *  Author: coldtobi
 *      LICENSE is BSD-2 clause.
 */

#include "archive_reader.hpp"
#include "archive_writer.hpp"
#include "archive_exception.hpp"

#include <gtest/gtest.h>
#include <string>
#include <set>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>

namespace ar = ns_archive::ns_reader;

class LibArchiveWrapperTest : public testing::Test {
public:
  LibArchiveWrapperTest() = default;
  virtual ~LibArchiveWrapperTest() {
    if (!HasFailure()) {
      for (auto const &x : cleanup_files) {
        ::unlink(x.c_str());
      }
    }
  }

  // Let the test know about archives to de deleted after the test
  // (will only be deleted if it succeded, not on failures.)
  void deleteAfterTest(const std::string &cleanup) {
    cleanup_files.insert(cleanup);
  }

  std::string getResourceDir() {
    auto resources = ::getenv("TEST_RESOURCES");
    if(!resources) return "";
    std::string ret(resources);
    if(ret.size() && (ret.at(ret.size()-1) != '/')) { ret += '/'; }
    return ret;
  }

  std::string getcwd() {
    auto cwd = ::getcwd(nullptr, 0);
    if(!cwd) { return "/tmp/"; }
    std::string ret(cwd);
    if(ret.size() && (ret.at(ret.size()-1) != '/')) { ret += '/'; }
    free(cwd);
    return ret;
  }

protected:
  std::set<std::string> cleanup_files;

};

TEST_F(LibArchiveWrapperTest, TestReadSimpleArchive) {
    // Reading an archive.
    auto dutarchive = getResourceDir() + "test1.tar.gz";

    std::cerr << dutarchive << std::endl;

    std::set<std::string> expected_files{"file1_random", "file2_random", "file3_zeros"};
    try
     {
       std::ifstream fs(dutarchive);
       ns_archive::reader reader = ns_archive::reader::make_reader<ar::format::_ALL, ar::filter::_ALL>(fs, 32000);

       for(auto entry : reader)
       {
         std::string filename = entry->get_header_value_pathname();
         std::string comparefile = getResourceDir() + filename;
         bool isdirectory = (filename.size() && filename[filename.size()-1] == '/');

         auto it = expected_files.find(filename);
         ASSERT_TRUE(it != expected_files.end());
         struct stat sb;
         auto statret = stat(comparefile.c_str(),&sb);
         EXPECT_EQ(0, statret);

         EXPECT_TRUE(S_ISDIR(sb.st_mode) == isdirectory);

         if (!isdirectory) {
           EXPECT_EQ(sb.st_size, entry->get_header_value_size());
           auto original = std::ifstream(comparefile, std::ios::in | std::ios::binary);
           auto *buf = new char[sb.st_size];
           auto *buf2 = new char[sb.st_size];
           auto &entrystream = entry->get_stream();
           original.read(buf, sb.st_size);
           entrystream.read(buf2, sb.st_size);
           EXPECT_TRUE(original.good());
           EXPECT_TRUE(entrystream.good());
           EXPECT_EQ(0,::memcmp(buf, buf2, sb.st_size));
           delete[] buf, buf2;
         }
         expected_files.erase(filename);
       }
     }
     catch(ns_archive::archive_exception& e)
     {
       ADD_FAILURE() << e.what();
     }

     EXPECT_EQ(0, expected_files.size());
}


TEST_F(LibArchiveWrapperTest, TestReadDirectoryArchive) {
    // Reading an archive.
    auto dutarchive = getResourceDir() + "test2.tar.gz";

    std::set<std::string> expected_files{"dir/", "dir/file1_random", "dir/file2_random", "dir/file3_zeros"};

    try
    {
      std::ifstream fs(dutarchive);
      ns_archive::reader reader = ns_archive::reader::make_reader<ar::format::_ALL, ar::filter::_ALL>(fs, 32000);

      for(auto entry : reader)
      {
        std::string filename = entry->get_header_value_pathname();
        std::string comparefile = getResourceDir() + filename;

        bool isdirectory = (filename.size() && filename[filename.size()-1] == '/');

        auto it = expected_files.find(filename);
        ASSERT_TRUE(it != expected_files.end());
        struct stat sb;
        auto statret = stat(comparefile.c_str(),&sb);
        EXPECT_EQ(0, statret);

        EXPECT_TRUE(S_ISDIR(sb.st_mode) == isdirectory);

        if (!isdirectory) {
          EXPECT_EQ(sb.st_size, entry->get_header_value_size());
          auto original = std::ifstream(comparefile, std::ios::in | std::ios::binary);
          auto *buf = new char[sb.st_size];
          auto *buf2 = new char[sb.st_size];
          auto &entrystream = entry->get_stream();
          original.read(buf, sb.st_size);
          entrystream.read(buf2, sb.st_size);
          EXPECT_TRUE(original.good());
          EXPECT_TRUE(entrystream.good());
          EXPECT_EQ(0,::memcmp(buf, buf2, sb.st_size));
          delete[] buf, buf2;
        }
        expected_files.erase(filename);
      }
    }
    catch(ns_archive::archive_exception& e)
    {
      ADD_FAILURE() << e.what();
    }

    EXPECT_EQ(0, expected_files.size());
}

TEST_F(LibArchiveWrapperTest, TestCreateDirectoryArchive) {
  auto dutarchive = getcwd() + "test_create.tar.gz";

  std::cerr << "DUT: " << dutarchive << std::endl;
  std::set<std::string> expected_files{"dir/file1_random", "dir/file2_random", "dir/file3_zeros"};

  // Create an archive.
  try {
    std::ofstream outfs(dutarchive, std::ios::trunc | std::ios::out);
    ns_archive::writer writer = ns_archive::writer::make_writer<ns_archive::ns_writer::format::_TAR, ns_archive::ns_writer::filter::_GZIP>(outfs, 10240);

    for(const auto& x: expected_files) {
      std::ifstream file(getResourceDir() + x);
      ASSERT_TRUE(file.good());
      ns_archive::entry out_entry(file);
      out_entry.set_header_value_pathname(x);
      writer.add_entry(out_entry);
    }
  } catch (ns_archive::archive_exception& e)
  {
    ADD_FAILURE() << e.what();
  }

  // Read back the archive and compare the files.
  try
     {
       std::ifstream fs(dutarchive);
       deleteAfterTest(dutarchive);
       ns_archive::reader reader = ns_archive::reader::make_reader<ar::format::_ALL, ar::filter::_ALL>(fs, 32000);

       for(auto entry : reader)
       {
         std::string filename = entry->get_header_value_pathname();
         std::string comparefile = getResourceDir() + filename;

         bool isdirectory = (filename.size() && filename[filename.size()-1] == '/');

         auto it = expected_files.find(filename);
         ASSERT_TRUE(it != expected_files.end());
         struct stat sb;
         auto statret = stat(comparefile.c_str(),&sb);
         EXPECT_EQ(0, statret);

         EXPECT_TRUE(S_ISDIR(sb.st_mode) == isdirectory);

         if (!isdirectory) {
           EXPECT_EQ(sb.st_size, entry->get_header_value_size());
           auto original = std::ifstream(comparefile, std::ios::in | std::ios::binary);
           auto *buf = new char[sb.st_size];
           auto *buf2 = new char[sb.st_size];
           auto &entrystream = entry->get_stream();
           original.read(buf, sb.st_size);
           entrystream.read(buf2, sb.st_size);
           EXPECT_TRUE(original.good());
           EXPECT_TRUE(entrystream.good());
           EXPECT_EQ(0,::memcmp(buf, buf2, sb.st_size));
           delete[] buf, buf2;
         }
         expected_files.erase(filename);
       }
     }
     catch(ns_archive::archive_exception& e)
     {
       ADD_FAILURE() << e.what() << " Archive: " << dutarchive;
     }

     EXPECT_EQ(0, expected_files.size());
     std::cerr << dutarchive << std::endl;

}
