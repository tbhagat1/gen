#pragma once

#include <sstream>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <map>
#include <error_code.hpp>
#include <boost/json.hpp>

namespace rates {
namespace framework {

  class index {
  public:

    using ptr         = std::shared_ptr<index>;
    using index_pair  = std::pair<std::string, std::string>;
    using index_pairs = std::vector<index_pair>;

    const std::string& type() const;
    const std::string& alias() const;
    const index_pairs& get_index_pairs() const;

    void type(const std::string&);
    void alias(const std::string&);
    void push_back(const std::string& name, const std::string& type);

  private:

    std::string  type_;
    std::string  alias_;
    index_pairs  index_pairs_;
  };
  using indices = std::vector<index::ptr>;

  inline const std::string&
  index::
  type() const {
    return type_;
  }

  inline const std::string&
  index::
  alias() const {
    return alias_;
  }

  inline const index::index_pairs&
  index::
  get_index_pairs() const {
    return index_pairs_;
  }

  inline void
  index::
  type(const std::string& typ) {
    type_ = typ;
  }

  inline void
  index::
  alias(const std::string& als) {
    alias_ = als;
  }

  inline void
  index::
  push_back(const std::string& name,
            const std::string& typ) {
    index_pairs_.emplace_back(std::make_pair(name, typ));
  }

  class field {
  public:

    const std::string& name() const;
    const std::string& type() const;
    const std::string& ref_name() const;
    size_t size() const;
    const std::string& db_name() const;

    void name(const std::string&);
    void type(const std::string&);
    void ref_name(const std::string&);
    void size(size_t sz);
    void db_name(const std::string&);
    using ptr = std::shared_ptr<field>;

  private:

    size_t       size_;
    std::string  name_;
    std::string  type_;
    std::string  ref_name_;
    std::string  db_name_;
  };
  using fields = std::vector<field::ptr>;

  inline const std::string&
  field::
  name() const {
    return name_;
  }

  inline const std::string&
  field::
  type() const {
    return type_;
  }

  inline const std::string&
  field::
  ref_name() const {
    return ref_name_;
  }

  inline size_t
  field::
  size() const {
    return size_;
  }

  inline const std::string&
  field::
  db_name() const {
    return db_name_;
  }

  inline void
  field::
  name(const std::string& name) {
    name_ = name;
  }

  inline void
  field::
  type(const std::string& typ) {
    type_ = typ;
  }

  inline void
  field::
  ref_name(const std::string& name) {
    ref_name_ = name;
  }

  inline void
  field::
  size(size_t sz) {
    size_ = sz;
  }

  inline void
  field::
  db_name(const std::string& name) {
    db_name_ = name;
  }

  class stored_proc {
  public:

    using parameter  = std::pair<std::string, std::string>;
    using parameters = std::vector<parameter>;
    using ptr        = std::shared_ptr<stored_proc>;

    void name(const std::string& name);
    void type(const std::string& typ);
    void push_back(const std::string&, const std::string&);

    const std::string& name() const;
    const std::string& type() const;
    const parameters& get_parameters() const;

  private:

    std::string  name_;
    std::string  type_;
    parameters   parameters_;
  };
  using stored_procs = std::map<std::string, stored_proc::ptr>;

  inline void
  stored_proc::
  name(const std::string& name) {
    name_ = name;
  }

  inline void
  stored_proc::
  type(const std::string& type) {
    type_ = type;
  }

  inline void
  stored_proc::
  push_back(const std::string& name,
            const std::string& type) {
    parameters_.push_back(std::make_pair(name, type));
  }

  inline const std::string&
  stored_proc::
  name() const {
    return name_;
  }

  inline const std::string&
  stored_proc::
  type() const {
    return type_;
  }

  inline const stored_proc::parameters&
  stored_proc::
  get_parameters() const {
    return parameters_;
  }

  class component : public std::enable_shared_from_this<component> {
  public:

    using ptr = std::shared_ptr<component>;

    const std::string& class_name() const;
    const fields& get_fields() const;
    const indices& get_indices() const;
    const stored_procs& get_stored_procs() const;

    void class_name(const std::string& name);
    void push_back(field::ptr);
    void push_back(index::ptr);
    void insert(stored_proc::ptr);
    void generate();

  private:

    void declare_prologue();

    bool           needs_mapping_;
    std::string    class_name_;
    fields         fields_;
    indices        indices_;
    stored_procs   stored_procs_;
    std::ofstream  ofs_;
  };
  using components = std::vector<component::ptr>;

  inline const std::string&
  component::
  class_name() const {
    return class_name_;
  }

  inline const fields&
  component::
  get_fields() const {
    return fields_;
  }

  inline const indices&
  component::
  get_indices() const {
    return indices_;
  }

  inline const stored_procs&
  component::
  get_stored_procs() const {
    return stored_procs_;
  }

  inline void
  component::
  class_name(const std::string& name) {
    class_name_ = name;
  }

  inline void
  component::
  push_back(field::ptr fld) {
    fields_.push_back(fld);
  }

  inline void
  component::
  push_back(index::ptr ndx) {
    indices_.push_back(ndx);
  }

  inline void
  component::
  insert(stored_proc::ptr sp) {
    stored_procs_.insert(std::pair(sp->type(), sp));
  }

  class parser {
  public:

    void parse(const std::string& fname);
    void generate();

  private:

    void parse_fields(component::ptr comp,
                      const boost::json::value& val);

    field::ptr parse_field(const boost::json::value& val);

    void parse_indices(component::ptr comp,
                       const boost::json::value& val);

    index::ptr parse_index(const boost::json::value& val);

    void parse_stored_procs(component::ptr comp,
                            const boost::json::value& val);
    stored_proc::ptr parse_stored_proc(const boost::json::value& val);

    components components_;
  };

  inline void
  parser::
  parse(const std::string& fname) {
  
    std::ifstream ifs(fname);
    std::stringstream ss;
    ss << ifs.rdbuf();

    boost::json::stream_parser par;
    boost::json::error_code ec;
    const std::string s = ss.str();
    par.write(s.c_str(), s.size(), ec);
    if (ec) {
      std::cout << "parsing failed: " << ec << std::endl;
      return;
    }
    par.finish(ec);
    if (ec) {
      std::cout << "finish failed: " << ec << std::endl;
      return;
    }
    auto node = par.release();
    auto top = node.get_object();
    auto i = top.begin();
    auto j = top.end();
    for (; i != j; ++i) {
      std::string class_name = i->key();
      component::ptr comp = std::make_shared<component>();
      comp->class_name(class_name);

      auto kids = i->value().get_object();
      auto p = kids.begin();
      auto q = kids.end();
      for (; p != q; ++p) {
        std::string key = p->key();
        if (key == "fields") {
          parse_fields(comp, p->value());
        }
        else if (key == "indices") {
          parse_indices(comp, p->value());
        }
        else if (key == "stored_procs") {
          parse_stored_procs(comp, p->value());
        }
      }
      components_.push_back(comp);
    }
  }

  inline void
  parser::
  generate() {
    for (auto comp : components_) {
      comp->generate();
    }
  }

  inline void
  parser::
  parse_fields(component::ptr comp,
               const boost::json::value& val) {

    auto node = val.get_array();
    auto p = node.begin();
    auto q = node.end();

    for (; p != q; ++p) {
      field::ptr fld = parse_field(*p);
      if (! fld) {
        std::cout << "Failed to make field" << std::endl;
        return;
      }
      comp->push_back(fld);
    }
    std::cout << comp->get_fields().size() << std::endl;
    return;
  }

  inline field::ptr
  parser::
  parse_field(const boost::json::value& val) {

    auto node = val.get_object();
    auto p = node.begin();
    auto q = node.end();

    field::ptr fld = std::make_shared<field>();
    for (; p != q; ++p) {

      std::string key = p->key();
      std::string val = boost::json::value_to<std::string>(p->value());

      if (key == "name") {
        fld->name(val);
      }
      else if (key == "type") {
        fld->type(val);
      }
      else if (key == "ref-name") {
        fld->ref_name(val);
      }
      else if (key == "size") {
        fld->size(::atoi(val.c_str()));
      }
      else if (key == "db_name") {
        fld->db_name(val);
      }
    }
    return fld;
  }

  inline void
  parser::
  parse_indices(component::ptr comp,
                const boost::json::value& val) {

    auto node = val.get_array();
    auto p = node.begin();
    auto q = node.end();

    for (; p != q; ++p) {
      index::ptr ndx = parse_index(*p);
      if (! ndx) {
        std::cout << "Failed to make index" << std::endl;
        return;
      }
      comp->push_back(ndx);
    }
    std::cout << comp->get_indices().size() << std::endl;
  }

  inline index::ptr
  parser::
  parse_index(const boost::json::value& val) {

    auto node = val.get_object();
    auto p = node.begin();
    auto q = node.end();

    index::ptr ndx = std::make_shared<index>();
    for (; p != q; ++p) {
      std::string key = p->key();
      if (key == "type") {
        std::string val = boost::json::value_to<std::string>(p->value());
        ndx->type(val);
      }
      else if (key == "alias") {
        std::string val = boost::json::value_to<std::string>(p->value());
        ndx->alias(val);
      }
      else if (key == "keys") {
        auto key_node = p->value().get_object();
        auto r = key_node.begin();
        auto s = key_node.end();
        for (; r != s; ++r) {
          std::string name = r->key();
          std::string type = boost::json::value_to<std::string>(r->value());
          ndx->push_back(name, type);
        }
      }
    }
    return ndx;
  }

  inline void
  parser::
  parse_stored_procs(component::ptr comp,
                     const boost::json::value& val) {

    auto node = val.get_array();
    auto p = node.begin();
    auto q = node.end();
    for (; p != q; ++p) {
      stored_proc::ptr sp = parse_stored_proc(*p);
      if (! sp) {
        std::cout << "Failed to make sp" << std::endl;
        return;
      }
      comp->insert(sp);
    }
  }

  inline stored_proc::ptr
  parser::
  parse_stored_proc(const boost::json::value& val) {

    auto node = val.get_object();
    auto p = node.begin();
    auto q = node.end();

    stored_proc::ptr sp = std::make_shared<stored_proc>();
    for (; p != q; ++p) {

      std::string key = p->key();
      if (key == "name") {
        std::string name = boost::json::value_to<std::string>(p->value());
        sp->name(name);
      }
      else if (key == "type") {
        std::string type = boost::json::value_to<std::string>(p->value());
        sp->type(type);
      }
      else if (key == "parameters") {
        auto pnode = p->value().get_object();
        auto r = pnode.begin();
        auto s = pnode.end();
        for (; r != s; ++r) {
          std::string pname = r->key();
          std::string ptype = boost::json::value_to<std::string>(r->value());
          sp->push_back(pname, ptype);
        }
      }
    }
    return sp;
  }

  class instance_maker {
  public:

    instance_maker(std::ofstream& ofs, component::ptr comp);
    void make();

  private:

    void declare_class();
    void declare_constructors();
    void declare_accessors();
    void declare_mutators();
    void declare_bind();
    void declare_members();
    void implement_constructors();
    void implement_accessors();
    void implement_mutators();
    void implement_bind();

    std::ofstream&  ofs_;
    component::ptr  component_;
  };

  class mapping_maker {
  public:

    mapping_maker(std::ofstream& ofs, component::ptr comp);
    void make();

  private:

    void declare_class();
    void declare_singleton_accessor();
    void declare_load();
    void declare_finders();
    void declare_members();
    void implement_singleton_accessor();
    void implement_load();
    void implement_finders();

    std::ofstream&  ofs_;
    component::ptr  component_;
  };

  inline void
  component::
  generate() {

    std::string path = "./" + class_name_ + ".hpp";
    ofs_.open(path);
    declare_prologue();

    instance_maker im(ofs_, shared_from_this());
    im.make();
    // if (needs_mapping_) {
      mapping_maker mm(ofs_, shared_from_this());
      mm.make();
    // }
    ofs_ << "}}" << std::endl;
    ofs_.close();
  }

  inline void
  component::
  declare_prologue() {

    ofs_ << "#pragma once" << std::endl << std::endl
         << "#include <set>" << std::endl
         << "#include <string>" << std::endl
         << "#include <memory>" << std::endl
         << "#include <mutex>" << std::endl
         << "#include <boost/multi_index_container.hpp>" << std::endl
         << "#include <boost/multi_index/member.hpp>" << std::endl
         << "#include <boost/multi_index/mem_fun.hpp>" << std::endl
         << "#include <boost/multi_index/hashed_index.hpp>" << std::endl
         << "#include <boost/multi_index/ordered_index.hpp>" << std::endl
         << "#include <boost/multi_index/composite_key.hpp>" << std::endl
         << "#include <boost/multi_index/indexed_by.hpp>" << std::endl
         << "#include <db/connection.hpp>"
         << std::endl << std::endl
         << "namespace rates {" << std::endl
         << "namespace generated {" << std::endl << std::endl
         << "  /// namespace shortening for boost multi-index" << std::endl
         << "  namespace mti = boost::multi_index;" << std::endl << std::endl;
  }

  inline
  instance_maker::
  instance_maker(std::ofstream& ofs,
                 component::ptr comp) :
    ofs_(ofs),
    component_(comp)
  {}
    
  inline void
  instance_maker::
  make() {
    declare_class();
    declare_constructors();
    declare_accessors();
    declare_mutators();
    declare_bind();
    declare_members();
    implement_constructors();
    implement_accessors();
    implement_mutators();
    implement_bind();
  }

  inline void
  instance_maker::
  declare_class() {

    std::string class_name = component_->class_name();
    ofs_ << "  //////" << std::endl
         << "  /// class " << class_name << std::endl
         << "  //////" << std::endl
         << "  class " << class_name << " {" << std::endl
         << "  public:" << std::endl << std::endl
         << "    //////" << std::endl
         << "    /// the one shared ptr type" << std::endl
         << "    //////" << std::endl
         << "    using ptr = std::shared_ptr<"
         << class_name
         << ">;"
         << std::endl << std::endl;
  }

  inline void
  instance_maker::
  declare_constructors() {

    std::string class_name = component_->class_name();
    ofs_ << "    //////" << std::endl
         << "    /// default constructor" << std::endl
         << "    //////" << std::endl
         << "    " << class_name << "();" << std::endl << std::endl
         << "    //////" << std::endl
         << "    /// parameter constructor" << std::endl
         << "    //////" << std::endl;

    std::string ctor = "    " + class_name + "(";
    std::size_t ctor_len = ctor.size();
    ofs_ << ctor;
    auto p = component_->get_fields().begin();
    auto q = component_->get_fields().end();
    size_t i = 0;
    size_t n = std::distance(p, q);
    for (; p != q; ++p, ++i) {
      field::ptr fp = *p;
      if (i != 0) {
        ofs_ << std::string(ctor_len, ' ');
      }
      if (fp->type() == "std::string") {
        ofs_ << "const std::string& ";
      }
      else {
        ofs_ << fp->type();
      }
      ofs_ << " " << fp->name();
      if (i < n - 1) {
        ofs_ << ",";
      }
      else {
        ofs_ << ");";
      }
      ofs_ << std::endl;
    }
    ofs_ << std::endl;
  }

  inline void
  instance_maker::
  declare_accessors() {

    ofs_ << "    //////" << std::endl
         << "    /// accessors" << std::endl
         << "    //////" << std::endl;

    auto p = component_->get_fields().begin();
    auto q = component_->get_fields().end();

    for (; p != q; ++p) {
      field::ptr fp = *p;
      if (fp->type() == "std::string") {
        ofs_ << "    const std::string& ";
      }
      else {
        ofs_ << "    " << fp->type() << " ";
      }
      ofs_ << fp->name() << "() const;" << std::endl;
    }
    ofs_ << std::endl;
  }

  inline void
  instance_maker::
  declare_mutators() {

    ofs_ << "    //////" << std::endl
         << "    /// mutators" << std::endl
         << "    //////" << std::endl;

    auto p = component_->get_fields().begin();
    auto q = component_->get_fields().end();
    for (; p != q; ++p) {
      field::ptr fp = *p;
      ofs_ << "    void " << fp->name() << "(";
      if (fp->type() == "std::string") {
        ofs_ << "const std::string&";
      }
      else {
        ofs_ << fp->type();
      }
      ofs_ << ");" << std::endl;
    }
    ofs_ << std::endl;
  }

  inline void
  instance_maker::
  declare_bind() {

    ofs_ << "    //////" << std::endl
         << "    /// bind" << std::endl
         << "    //////" << std::endl
         << "    void bind(connection_ptr conn);"
         << std::endl << std::endl;
  }

  inline void
  instance_maker::
  declare_members() {

    ofs_ << "  private:" << std::endl << std::endl
         << "    //////" << std::endl
         << "    /// class members" << std::endl
         << "    //////" << std::endl;

    auto p = component_->get_fields().begin();
    auto q = component_->get_fields().end();
    const size_t flen = std::string("std::string").size();
    for (; p != q; ++p) {
      field::ptr fp = *p;
      std::string type = fp->type();
      ofs_ << "    " << type << std::string(flen - type.size(), ' ')
           << "  " << fp->name() << "_;" << std::endl;
    }
    ofs_ << "  };" << std::endl << std::endl;
  }

  inline void
  instance_maker::
  implement_constructors() {

    ofs_ << "  //////" << std::endl
         << "  /// default constructor" << std::endl
         << "  //////" << std::endl;

    std::string class_name = component_->class_name();
    ofs_ << "  inline" << std::endl
         << "  " << class_name << "::" << std::endl
         << "  " << class_name << "() : " << std::endl;

    auto p = component_->get_fields().begin();
    auto q = component_->get_fields().end();
    size_t i = 0;
    size_t n = std::distance(p, q);
    for (; p != q; ++p, ++i) {

      field::ptr fld = *p;
      std::string name = fld->name();
      std::string type = fld->type();
      size_t size = fld->size();
      ofs_ << "    " << name << "_(";
      if (type == "std::string") {
        ofs_ << size << ", '\\0')";
      }
      else {
        ofs_ << "0)";
      }
      if (i < n - 1) {
        ofs_ << ",";
      }
      else {
        ofs_ << " {";
      }
      ofs_ << std::endl;
    }
    ofs_ << "  }" << std::endl << std::endl;

    ofs_ << "  //////" << std::endl
         << "  /// member constructor" << std::endl
         << "  //////" << std::endl;

    ofs_ << "  inline" << std::endl
         << "  " << class_name << "::" << std::endl
         << "  " << class_name << "(";

    std::string ctor = "  " + class_name + "(";
    std::size_t ctor_len = ctor.size();
    p = component_->get_fields().begin();
    q = component_->get_fields().end();
    i = 0;
    for (; p != q; ++p, ++i) {
      field::ptr fp = *p;
      if (i != 0) {
        ofs_ << std::string(ctor_len, ' ');
      }
      if (fp->type() == "std::string") {
        ofs_ << "const std::string& ";
      }
      else {
        ofs_ << fp->type();
      }
      ofs_ << " " << fp->name();
      if (i < n - 1) {
        ofs_ << ",";
      }
      else {
        ofs_ << ") : ";
      }
      ofs_ << std::endl;
    }
    p = component_->get_fields().begin();
    q = component_->get_fields().end();
    i = 0;
    for (; p != q; ++p, ++i) {
      field::ptr fp = *p;
      ofs_ << "    " << fp->name() << "_("
           << fp->name()
           << ")";
      if (i < n - 1) {
        ofs_ << ",";
      }
      else {
        ofs_ << " {";
      }
      ofs_ << std::endl;
    }
    ofs_ << "  }" << std::endl << std::endl;
  }

  inline void
  instance_maker::
  implement_accessors() {

    std::string class_name = component_->class_name();
    auto p = component_->get_fields().begin();
    auto q = component_->get_fields().end();

    ofs_ << "  //////" << std::endl
         << "  /// accessors" << std::endl
         << "  //////" << std::endl << std::endl;

    for (; p != q; ++p) {

      field::ptr fp = *p;
      ofs_ << "  inline ";
      if (fp->type() == "std::string") {
        ofs_ << "const std::string& ";
      }
      else {
        ofs_ << fp->type();
      }
      ofs_ << std::endl;
      ofs_ << "  " << class_name << "::" << std::endl
           << "  " << fp->name() << "() const {" << std::endl
           << "    return " << fp->name() << "_;" << std::endl
           << "  }" << std::endl << std::endl;
    }
  }

  inline void
  instance_maker::
  implement_mutators() {

    std::string class_name = component_->class_name();
    auto p = component_->get_fields().begin();
    auto q = component_->get_fields().end();

    ofs_ << "  //////" << std::endl
         << "  /// mutators" << std::endl
         << "  //////" << std::endl << std::endl;

    for (; p != q; ++p) {

      field::ptr fp = *p;
      ofs_ << "  inline void" << std::endl
           << "  " << class_name << "::" << std::endl
           << "  " << fp->name() << "(";
      if (fp->type() == "std::string") {
        ofs_ << "const std::string& ";
      }
      else {
        ofs_ << fp->type() << " ";
      }
      ofs_ << fp->name() << ") {" << std::endl
           << "    " << fp->name() << "_ = "
           << fp->name() << ";" << std::endl
           << "  }" << std::endl << std::endl;
    }
  }

  inline void
  instance_maker::
  implement_bind() {

    std::string class_name = component_->class_name();

    ofs_ << "  //////" << std::endl
         << "  /// bind" << std::endl
         << "  //////" << std::endl
         << "  inline void " << std::endl
         << "  " << class_name << "::" << std::endl
         << "  bind(connection_ptr conn) {"
         << std::endl << std::endl;

    auto p = component_->get_fields().begin();
    auto q = component_->get_fields().end();
    for (; p != q; ++p) {

      field::ptr fp = *p;
      ofs_ << "    conn->genericBind("
           << "\"" << fp->db_name() << "\""
           << ", ";
      if (fp->type() == "std::string") {
        ofs_ << "&"
             << fp->name()
             << "_"
             << "[0]";
      }
      else {
        ofs_ << fp->name() << "_";
      }
      ofs_ << ");" << std::endl;
    }
    ofs_ << "  }" << std::endl << std::endl;
  }

  inline
  mapping_maker::
  mapping_maker(std::ofstream& ofs,
                component::ptr comp) :
    ofs_(ofs),
    component_(comp) {
  }

  inline void
  mapping_maker::
  make() {
    declare_class();
    declare_singleton_accessor();
    declare_load();
    declare_finders();
    declare_members();
    implement_singleton_accessor();
    implement_load();
    implement_finders();
  }

  inline void
  mapping_maker::
  declare_class() {

    std::string class_name = component_->class_name();
    ofs_ << "  //////" << std::endl
         << "  /// class " << class_name << "_mapping" << std::endl
         << "  //////" << std::endl
         << "  class " << class_name << "_mapping {" << std::endl
         << "  public:" << std::endl;
    ofs_ << std::endl;
  }

  inline void
  mapping_maker::
  declare_singleton_accessor() {

    std::string class_name = component_->class_name();
    ofs_ << "    //////" << std::endl
         << "    /// singleton accessor" << std::endl
         << "    //////" << std::endl
         << "    static " << class_name << "_mapping& instance();" << std::endl;
    ofs_ << std::endl;
  }

  inline void
  mapping_maker::
  declare_load() {

    std::string class_name = component_->class_name();
    ofs_ << "    //////" << std::endl
         << "    /// load" << std::endl
         << "    //////" << std::endl
         << "    bool load(connection_ptr);" << std::endl;
    ofs_ << std::endl;
  }

  inline void
  mapping_maker::
  declare_finders() {

    std::string class_name = component_->class_name();
    ofs_ << "    //////" << std::endl
         << "    /// finder methods" << std::endl
         << "    //////" << std::endl;

    auto p = component_->get_indices().begin();
    auto q = component_->get_indices().end();
    for (; p != q; ++p) {
      index::ptr ndx = *p;
      std::string line = "    " + class_name + "::ptr find_by_" + ndx->alias() + "(";
      size_t len = line.size();
      ofs_ << line;

      auto a = ndx->get_index_pairs().begin();
      auto b = ndx->get_index_pairs().end();
      size_t i = 0;
      size_t n = std::distance(a, b);
      for (; a != b; ++a, ++i) {

        std::string index_name = a->first;
        std::string index_type = a->second;       
        if (i != 0) {
          ofs_ << std::string(len, ' ');
        }
        if (index_type == "std::string") {
          ofs_ << "const std::string& ";
        }
        else {
          ofs_ << index_type << " ";
        }
        ofs_ << index_name;
        if (i < n - 1) {
          ofs_ << ",";
        }
        else {
          ofs_ << ");";
        }
        ofs_ << std::endl;
      }
    }
    ofs_ << std::endl;
  }

  inline void
  mapping_maker::
  declare_members() {

    std::string class_name = component_->class_name();
    ofs_ << "  private:" << std::endl << std::endl
         << "    //////" << std::endl
         << "    /// default constructor" << std::endl
         << "    //////" << std::endl
         << "    " << class_name << "_mapping"
         << "();" << std::endl << std::endl;

    ofs_ << "    //////" << std::endl
         << "    /// boost multi-index tag definitions" << std::endl
         << "    //////" << std::endl;

    auto a = component_->get_indices().begin();
    auto b = component_->get_indices().end();
    for (; a != b; ++a) {
      index::ptr ndx = *a;
      ofs_ << "    struct " << ndx->alias() << "_tag {};" << std::endl;
    }
    ofs_ << std::endl;
    ofs_ << "    //////" << std::endl
         << "    /// boost multi-index definition" << std::endl
         << "    //////" << std::endl;
    ofs_ << "    typedef mti::multi_index_container<" << std::endl
         << "      " << class_name << "::ptr," << std::endl
         << "      mti::indexed_by<" << std::endl;

    a = component_->get_indices().begin();
    b = component_->get_indices().end();
    size_t n = std::distance(a, b);
    for (size_t i = 0; a != b; ++a, ++i) {

      index::ptr ndx = *a;
      std::string type = ndx->type();
      std::string alias = ndx->alias();
      size_t nkeys = ndx->get_index_pairs().size();
      bool composite = nkeys > 1;
      if (type == "ordered-unique") {
        ofs_ << "        mti::ordered_unique<" << std::endl;
      }
      else if (type == "ordered-non-unique") {
        ofs_ << "        mti::ordered_non_unique<" << std::endl;
      }
      else if (type == "hashed-unique") {
        ofs_ << "        mti::hashed_unique<" << std::endl;
      }
      else if (type == "hashed-non-unique") {
        ofs_ << "        mti::hashed_non_unique<" << std::endl;
      }
      ofs_ << "          mti::tag<"
           << alias
           << "_tag>,"
           << std::endl;
      if (! composite) {

        auto c = ndx->get_index_pairs().begin();
        std::string kname = c->first;
        std::string ktype = c->second;
        if (ktype == "std::string") {
          ktype = "const std::string&";
        }
        ofs_ << "          mti::const_mem_fun<"
             << class_name
             << ", "
             << ktype
             << ", &"
             << class_name
             << "::"
             << kname
             << ">"
             << std::endl;
      }
      else {
        ofs_ << "          mti::composite_key<"
             << std::endl
             << "            "
             << class_name
             << "::ptr,"
             << std::endl;

        auto c = ndx->get_index_pairs().begin();
        auto d = ndx->get_index_pairs().end();
        size_t dn = std::distance(c, d);

        for (size_t e = 0; c != d; ++c, ++e) {
          std::string kname = c->first;
          std::string ktype = c->second;
          if (ktype == "std::string") {
            ktype = "const std::string&";
          }
          ofs_ << "            mti::const_mem_fun<"
               << class_name
               << ", "
               << ktype
               << ", &"
               << class_name
               << "::"
               << kname
               << ">";
          if (e < dn - 1) {
            ofs_ << ",";
          }
          ofs_ << std::endl;
        }
        ofs_ << "          >" << std::endl;
      }
      ofs_ << "        >";
      if (i < n - 1) {
        ofs_ << ",";
      }
      ofs_ << std::endl;
    }
    ofs_ << "      >" << std::endl;
    ofs_ << "    > " << class_name << "_table;" << std::endl;
    ofs_ << std::endl;

    ofs_ << "    //////" << std::endl
         << "    /// boost multi-index indices" << std::endl
         << "    //////" << std::endl;

    a = component_->get_indices().begin();
    b = component_->get_indices().end();
    size_t mlen = 0;
    for (; a != b; ++a) {
      index::ptr ndx = *a;
      if (ndx->alias().size() > mlen) {
        mlen = ndx->alias().size();
      }
    }
    a = component_->get_indices().begin();
    b = component_->get_indices().end();
    for (; a != b; ++a) {
      index::ptr ndx = *a;
      ofs_ << "    using "
           << ndx->alias()
           << "_index"
           << std::string(mlen - ndx->alias().size(), ' ')
           << " = "
           << class_name
           << "_table::index<"
           << ndx->alias()
           << "_tag>::type;"
           << std::endl;
    }
    ofs_ << std::endl;
    ofs_ << "    //////" << std::endl
         << "    /// the "
         << class_name
         << " table itself" << std::endl
         << "    //////" << std::endl;
    ofs_ << "    "
         << class_name
         << "_table  "
         << class_name
         << "_table_;"
         << std::endl
         << std::endl;
    ofs_ << "    //////" << std::endl
         << "    /// synchronizes access to singleton data" << std::endl
         << "    //////" << std::endl
         << "    std::mutex  lock_;" << std::endl
         << "  };" << std::endl << std::endl;
  }

  inline void
  mapping_maker::
  implement_singleton_accessor() {

    std::string class_name = component_->class_name() + "_mapping";
    ofs_ << "  //////" << std::endl
         << "  /// singleton accessor" << std::endl
         << "  //////" << std::endl
         << "  inline " << class_name << "& " << std::endl
         << "  " << class_name << "::" << std::endl
         << "  instance() {" << std::endl
         << "    static " << class_name << " instance_;" << std::endl
         << "    return instance_;" << std::endl
         << "  }" << std::endl << std::endl;
  }

  inline void
  mapping_maker::
  implement_load() {

    std::string class_name = component_->class_name();
    ofs_ << "  //////" << std::endl
         << "  /// load" << std::endl
         << "  //////" << std::endl
         << "  inline bool" << std::endl
         << "  " << class_name << "_mapping" << "::" << std::endl
         << "  load(connection_ptr conn) {"
         << std::endl << std::endl
         << "    std::lock_guard<std::mutex>  guard(lock_);"
         << std::endl;

    const auto& sps = component_->get_stored_procs();
    auto i = sps.find("read");
    auto sp = i->second;
    ofs_ << "    "
         << "std::string sp = \"exec "
         << sp->name()
         << "\";"
         << std::endl;
    ofs_ << "    " << class_name << " area;" << std::endl
         << "    int result = conn->execute(sp);" << std::endl
         << "    if (result == FAIL) return false;" << std::endl << std::endl
         << "    area.bind(conn);" << std::endl
         << "    while (conn->nextRow() != NO_MORE_ROWS) {" << std::endl
         << "      " << class_name << "::ptr row = std::make_shared<"
         << class_name << ">(area);" << std::endl
         << "      " << class_name << "_table_.insert(row);" << std::endl
         << "    }"
         << std::endl
         << "    return true;" << std::endl
         << "  }" << std::endl << std::endl;
  }

  inline void
  mapping_maker::
  implement_finders() {

    std::string class_name = component_->class_name() + "_mapping";
    ofs_ << "  //////" << std::endl
         << "  /// finders" << std::endl
         << "  //////" << std::endl << std::endl;

    auto p = component_->get_indices().begin();
    auto q = component_->get_indices().end();
    for (; p != q; ++p) {

      index::ptr ndx = *p;
      std::string alias = ndx->alias();
      std::string line = "  find_by_" + alias + "(";
      size_t len = line.size();
      ofs_ << "  inline " << component_->class_name() + "::ptr " << std::endl
           << "  " << class_name << "::" << std::endl
           << line;

      auto c = ndx->get_index_pairs().begin();
      auto d = ndx->get_index_pairs().end();
      size_t i = 0;
      size_t n = std::distance(c, d);
      for (; c != d; ++c, ++i) {

        std::string name = c->first;
        std::string type = c->second;
        if (i != 0) {
          ofs_ << std::string(len, ' ');
        }
        if (type == "std::string") {
          ofs_ << "const std::string& ";
        }
        else {
          ofs_ << type << " ";
        }
        ofs_ << name;
        if (i < n - 1) {
          ofs_ << ",";
        }
        else {
          ofs_ << ") {";
        }
        ofs_ << std::endl;
      }
      ofs_ << std::endl;
      ofs_ << "    std::lock_guard<std::mutex>  guard(lock_);"
           << std::endl;
      ofs_ << "    const auto& p = "
           << component_->class_name() + "_table_.get<"
           << alias << "_tag>();"
           << std::endl
           << "    auto q = p.find(";
      bool is_composite = n > 1;
      if (is_composite) {
        ofs_ << "boost::make_tuple(";
      }
      c = ndx->get_index_pairs().begin();
      d = ndx->get_index_pairs().end();
      i = 0;
      for (; c != d; ++c, ++i) {

        std::string name = c->first;
        ofs_ << name;
        if (i < n - 1) {
          ofs_ << ",";
        }
        else {
          ofs_ << ")";
          if (is_composite) {
            ofs_ << ")";
          }
          ofs_ << ";";
        }
      }
      ofs_ << std::endl;
      ofs_ << "    return q != p.end() ? *q : "
           << component_->class_name()
           << "::ptr();"
           << std::endl
           << "  }"
           << std::endl << std::endl;
    }
  }

}}
