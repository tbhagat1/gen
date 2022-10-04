#pragma once

#include <set>
#include <string>
#include <memory>
#include <mutex>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <db/connection.hpp>

namespace rates {
namespace generated {

  /// namespace shortening for boost multi-index
  namespace mti = boost::multi_index;

  //////
  /// class position_source
  //////
  class position_source {
  public:

    //////
    /// the one shared ptr type
    //////
    using ptr = std::shared_ptr<position_source>;

    //////
    /// default constructor
    //////
    position_source();

    //////
    /// parameter constructor
    //////
    position_source(const std::string&  source,
                    const std::string&  type,
                    const std::string&  date,
                    int index);

    //////
    /// accessors
    //////
    const std::string& source() const;
    const std::string& type() const;
    const std::string& date() const;
    int index() const;

    //////
    /// mutators
    //////
    void source(const std::string&);
    void type(const std::string&);
    void date(const std::string&);
    void index(int);

    //////
    /// bind
    //////
    void bind(connection_ptr conn);

  private:

    //////
    /// class members
    //////
    std::string  source_;
    std::string  type_;
    std::string  date_;
    int          index_;
  };

  //////
  /// default constructor
  //////
  inline
  position_source::
  position_source() : 
    source_(64, '\0'),
    type_(64, '\0'),
    date_(64, '\0'),
    index_(0) {
  }

  //////
  /// member constructor
  //////
  inline
  position_source::
  position_source(const std::string&  source,
                  const std::string&  type,
                  const std::string&  date,
                  int index) : 
    source_(source),
    type_(type),
    date_(date),
    index_(index) {
  }

  //////
  /// accessors
  //////

  inline const std::string& 
  position_source::
  source() const {
    return source_;
  }

  inline const std::string& 
  position_source::
  type() const {
    return type_;
  }

  inline const std::string& 
  position_source::
  date() const {
    return date_;
  }

  inline int
  position_source::
  index() const {
    return index_;
  }

  //////
  /// mutators
  //////

  inline void
  position_source::
  source(const std::string& source) {
    source_ = source;
  }

  inline void
  position_source::
  type(const std::string& type) {
    type_ = type;
  }

  inline void
  position_source::
  date(const std::string& date) {
    date_ = date;
  }

  inline void
  position_source::
  index(int index) {
    index_ = index;
  }

  //////
  /// bind
  //////
  inline void 
  position_source::
  bind(connection_ptr conn) {

    conn->genericBind("position_source", &source_[0]);
    conn->genericBind("position_type", &type_[0]);
    conn->genericBind("position_type", &date_[0]);
    conn->genericBind("position_index", index_);
  }

  //////
  /// class position_source_mapping
  //////
  class position_source_mapping {
  public:

    //////
    /// singleton accessor
    //////
    static position_source_mapping& instance();

    //////
    /// load
    //////
    bool load(connection_ptr);

    //////
    /// finder methods
    //////
    position_source::ptr find_by_composite_key(const std::string& source,
                                               int index);
    position_source::ptr find_by_source(const std::string& source);
    position_source::ptr find_by_index(int index);

  private:

    //////
    /// default constructor
    //////
    position_source_mapping();

    //////
    /// boost multi-index tag definitions
    //////
    struct composite_key_tag {};
    struct source_tag {};
    struct index_tag {};

    //////
    /// boost multi-index definition
    //////
    typedef mti::multi_index_container<
      position_source::ptr,
      mti::indexed_by<
        mti::ordered_unique<
          mti::tag<composite_key_tag>,
          mti::composite_key<
            position_source::ptr,
            mti::const_mem_fun<position_source, const std::string&, &position_source::source>,
            mti::const_mem_fun<position_source, int, &position_source::index>
          >
        >,
        mti::ordered_non_unique<
          mti::tag<source_tag>,
          mti::const_mem_fun<position_source, const std::string&, &position_source::source>
        >,
        mti::ordered_non_unique<
          mti::tag<index_tag>,
          mti::const_mem_fun<position_source, int, &position_source::index>
        >
      >
    > position_source_table;

    //////
    /// boost multi-index indices
    //////
    using composite_key_index = position_source_table::index<composite_key_tag>::type;
    using source_index        = position_source_table::index<source_tag>::type;
    using index_index         = position_source_table::index<index_tag>::type;

    //////
    /// the position_source table itself
    //////
    position_source_table  position_source_table_;

    //////
    /// synchronizes access to singleton data
    //////
    std::mutex  lock_;
  };

  //////
  /// singleton accessor
  //////
  inline position_source_mapping& 
  position_source_mapping::
  instance() {
    static position_source_mapping instance_;
    return instance_;
  }

  //////
  /// load
  //////
  inline bool
  position_source_mapping::
  load(connection_ptr conn) {

    std::lock_guard<std::mutex>  guard(lock_);
    std::string sp = "exec vm_read_rate_source";
    position_source area;
    int result = conn->execute(sp);
    if (result == FAIL) return false;

    area.bind(conn);
    while (conn->nextRow() != NO_MORE_ROWS) {
      position_source::ptr row = std::make_shared<position_source>(area);
      position_source_table_.insert(row);
    }
    return true;
  }

  //////
  /// finders
  //////

  inline position_source::ptr 
  position_source_mapping::
  find_by_composite_key(const std::string& source,
                        int index) {

    std::lock_guard<std::mutex>  guard(lock_);
    const auto& p = position_source_table_.get<composite_key_tag>();
    auto q = p.find(boost::make_tuple(source,index));
    return q != p.end() ? *q : position_source::ptr();
  }

  inline position_source::ptr 
  position_source_mapping::
  find_by_source(const std::string& source) {

    std::lock_guard<std::mutex>  guard(lock_);
    const auto& p = position_source_table_.get<source_tag>();
    auto q = p.find(source);
    return q != p.end() ? *q : position_source::ptr();
  }

  inline position_source::ptr 
  position_source_mapping::
  find_by_index(int index) {

    std::lock_guard<std::mutex>  guard(lock_);
    const auto& p = position_source_table_.get<index_tag>();
    auto q = p.find(index);
    return q != p.end() ? *q : position_source::ptr();
  }

}}
