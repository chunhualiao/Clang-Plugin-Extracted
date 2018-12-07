#include <vector>
#include <cstddef>
#include <string>
#include <memory> // unique_ptr C++11 feature!!
#include <dlfcn.h> // dlopen()

//C++14 feature isolated here
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// -------------AST-------------------
struct Node {
  int val;
  Node* left;
  Node* right;

  Node(int v=0):val(v) {
    left=NULL;
    right=NULL;
  };
};

//----------ASTConsumer -----------------
class ASTConsumer{
public:
  ASTConsumer(){};

  virtual void process(Node*) {}; 
  virtual ~ASTConsumer(){} // must have virtual destructor
};

class PluginASTAction {
public:
  virtual std::vector<ASTConsumer*> CreateASTConsumer() = 0;

  /// \brief Parse the given plugin command line arguments.
  // virtual bool ParseArgs(const CompilerInstance &CI,
  //                      const std::vector<std::string> &arg) = 0;
  virtual ~PluginASTAction(){} // must have virtual destructor
};

#if 0
/// \brief Abstract base class to use for preprocessor-based frontend actions.
class PreprocessorFrontendAction : public FrontendAction {
protected:
  /// \brief Provide a default implementation which returns aborts;
  /// this method should never be called by FrontendAction clients.
  std::unique_ptr<ASTConsumer*> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef InFile) override;

public:
  bool usesPreprocessorOnly() const override { return true; }
};
#endif

//------------------iterator range----------------
/// \brief A range adaptor for a pair of iterators.
///
/// This just wraps two iterators into a range-compatible interface. Nothing
/// fancy at all.
template <typename IteratorT>
class iterator_range {
  IteratorT begin_iterator, end_iterator;

public:
  //TODO: Add SFINAE to test that the Container's iterators match the range's
  //      iterators.
  template <typename Container>
  iterator_range(Container &&c)
  //TODO: Consider ADL/non-member begin/end calls.
      : begin_iterator(c.begin()), end_iterator(c.end()) {}
  iterator_range(IteratorT begin_iterator, IteratorT end_iterator)
      : begin_iterator(std::move(begin_iterator)),
        end_iterator(std::move(end_iterator)) {}

  IteratorT begin() const { return begin_iterator; }
  IteratorT end() const { return end_iterator; }
};

/// \brief Convenience function for iterating over sub-ranges.
///
/// This provides a bit of syntactic sugar to make using sub-ranges
/// in for loops a bit easier. Analogous to std::make_pair().
template <class T> iterator_range<T> make_range(T x, T y) {
  return iterator_range<T>(std::move(x), std::move(y));
}

template <typename T> iterator_range<T> make_range(std::pair<T, T> p) {
  return iterator_range<T>(std::move(p.first), std::move(p.second));
}

template<typename T>
iterator_range<decltype(begin(std::declval<T>()))> drop_begin(T &&t, int n) {
  return make_range(std::next(begin(t), n), end(t));
}

//------------------------------------- Registry --------------
/// A simple registry entry which provides only a name, description, and
  /// no-argument constructor.
  template <typename T>
  class SimpleRegistryEntry {
    const char *Name, *Desc;
    std::unique_ptr<T> (*Ctor)();

  public:
    SimpleRegistryEntry(const char *N, const char *D, std::unique_ptr<T> (*C)())
      : Name(N), Desc(D), Ctor(C)
    {}

    const char *getName() const { return Name; }
    const char *getDesc() const { return Desc; }
    std::unique_ptr<T> instantiate() const { return Ctor(); }
    //T* instantiate() const { return Ctor(); }
  };

  /// Traits for registry entries. If using other than SimpleRegistryEntry, it
  /// is necessary to define an alternate traits class.
  template <typename T>
  class RegistryTraits {
    RegistryTraits() = delete;

  public:
    typedef SimpleRegistryEntry<T> entry;

    /// nameof/descof - Accessors for name and description of entries. These are
    //                  used to generate help for command-line options.
    static const char *nameof(const entry &Entry) { return Entry.getName(); }
    static const char *descof(const entry &Entry) { return Entry.getDesc(); }
  };

  /// A global registry used in conjunction with static constructors to make
  /// pluggable components (like targets or garbage collectors) "just work" when
  /// linked with an executable.
  template <typename T, typename U = RegistryTraits<T> >
  class Registry {
  public:
    typedef U traits;
    typedef typename U::entry entry;

    class node;
    class listener;
    class iterator;

  private:
    Registry() = delete;

    static void Announce(const entry &E) {
      for (listener *Cur = ListenerHead; Cur; Cur = Cur->Next)
        Cur->registered(E);
    }

    friend class node;
    static node *Head, *Tail;

    friend class listener;
    static listener *ListenerHead, *ListenerTail;

  public:
    /// Node in linked list of entries.
    ///
    class node {
      friend class iterator;

      node *Next;
      const entry& Val;

    public:
      node(const entry& V) : Next(nullptr), Val(V) {
        if (Tail)
          Tail->Next = this;
        else
          Head = this;
        Tail = this;

        Announce(V);
      }
    };

    /// Iterators for registry entries.
    ///
    class iterator {
      const node *Cur;

    public:
      explicit iterator(const node *N) : Cur(N) {}

      bool operator==(const iterator &That) const { return Cur == That.Cur; }
      bool operator!=(const iterator &That) const { return Cur != That.Cur; }
      iterator &operator++() { Cur = Cur->Next; return *this; }
      const entry &operator*() const { return Cur->Val; }
      const entry *operator->() const { return &Cur->Val; }
    };

    static iterator begin() { return iterator(Head); }
    static iterator end()   { return iterator(nullptr); }

    static iterator_range<iterator> entries() {
      return make_range(begin(), end());
    }

    /// Abstract base class for registry listeners, which are informed when new
    /// entries are added to the registry. Simply subclass and instantiate:
    ///
    /// \code
    ///   class CollectorPrinter : public Registry<Collector>::listener {
    ///   protected:
    ///     void registered(const Registry<Collector>::entry &e) {
    ///       cerr << "collector now available: " << e->getName() << "\n";
    ///     }
    ///
    ///   public:
    ///     CollectorPrinter() { init(); }  // Print those already registered.
    ///   };
    ///
    ///   CollectorPrinter Printer;
    /// \endcode
    class listener {
      listener *Prev, *Next;

      friend void Registry::Announce(const entry &E);

    protected:
      /// Called when an entry is added to the registry.
      ///
      virtual void registered(const entry &) = 0;

      /// Calls 'registered' for each pre-existing entry.
      ///
      void init() {
        for (iterator I = begin(), E = end(); I != E; ++I)
          registered(*I);
      }

    public:
      listener() : Prev(ListenerTail), Next(nullptr) {
        if (Prev)
          Prev->Next = this;
        else
          ListenerHead = this;
        ListenerTail = this;
      }

      virtual ~listener() {
        if (Next)
          Next->Prev = Prev;
        else
          ListenerTail = Prev;
        if (Prev)
          Prev->Next = Next;
        else
          ListenerHead = Next;
      }
    };

    /// A static registration template. Use like such:
    ///
    ///   Registry<Collector>::Add<FancyGC>
    ///   X("fancy-gc", "Newfangled garbage collector.");
    ///
    /// Use of this template requires that:
    ///
    ///  1. The registered subclass has a default constructor.
    //
    ///  2. The registry entry type has a constructor compatible with this
    ///     signature:
    ///
    ///       entry(const char *Name, const char *ShortDesc, T *(*Ctor)());
    ///
    /// If you have more elaborate requirements, then copy and modify.
    ///
    template <typename V>
    class Add {
      entry Entry;
      node Node;

      static std::unique_ptr<T> CtorFn() { return make_unique<V>(); }

    public:
      Add(const char *Name, const char *Desc)
        : Entry(Name, Desc, CtorFn), Node(Entry) {}
    };

    /// Registry::Parser now lives in llvm/Support/RegistryParser.h.
  };

  // Since these are defined in a header file, plugins must be sure to export
  // these symbols.

  template <typename T, typename U>
  typename Registry<T,U>::node *Registry<T,U>::Head;

  template <typename T, typename U>
  typename Registry<T,U>::node *Registry<T,U>::Tail;

  template <typename T, typename U>
  typename Registry<T,U>::listener *Registry<T,U>::ListenerHead;

  template <typename T, typename U>
  typename Registry<T,U>::listener *Registry<T,U>::ListenerTail;
 //---------------------------------------------------   

 extern template class Registry<PluginASTAction>; 
 typedef Registry<PluginASTAction> FrontendPluginRegistry;


  std::vector<ASTConsumer *> obtainConsumers (const char* lib_file_name, std::string action_name);
