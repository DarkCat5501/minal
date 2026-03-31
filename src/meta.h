#ifndef META_H_
#define META_H_

#define $MetaStr_Impl(X) #X
#define $MetaStr(X) $MetaStr_Impl(X)
#define $$Concat_Impl(a, b) a##b

#define $$Concat(a, b) $$Concat_Impl(a, b)
#define $$Concat2(a, b) $$Concat_Impl(a, b)
#define $$Concat3(a, b, c) $$Concat(a, $$Concat2(b, c))
#define $$Concat4(a, b, c, d) $$Concat2($$Concat2(a, b), $$Concat2(c, d))
#define $$Concat5(a, b, c, d, e) $$Concat2(a, $$Concat4(b, c, d, e))
#define $$Concat6(a, b, c, d, e, f) $$Concat2($$Concat2(a, b), $$Concat4(c, d, e, f))
#define $$Concat7(a, b, c, d, e, f, g) $$Concat2(a, $$Concat6(b, c, d, e, f, g))
#define $$Concat8(a, b, c, d, e, f, g, h) $$Concat2($$Concat4(a, b, c, d), $$Concat4(e, f, g, h))

#define $Id$ $$Concat(__LINE__, __COUNTER__)
#define $Var$(Name) $$Concat(Name, __LINE__)
#define $PVar$(Prefix, Name) $$Concat3(Prefix, Name, __LINE__)
#define $PId$(Prefix) $$Concat(Prefix, $Id$)

#define $MetaType$(SECTION) SECTION##_MetaType
#define $MetaStart$(SECTION) $$Concat(__start_,SECTION)
#define $MetaEnd$(SECTION) $$Concat(__stop_,SECTION)

#define $$Attr(...) __attribute__((__VA_ARGS__))

/////////////////////////
// Function attributes //
/////////////////////////


/** Declares to the compiler that a veriable or argument is maybe unused **/
#define $$MaybeUnused $$Attr(unused)

/** 
* Declares to the compiler the struct or enum should be packed
* meaning it will use the least amount of byte to represent the data
**/
#define $$Packed $$Attr(packed)

/** Declares to the compiler that a veriable or argument is maybe unused **/
#define $$NoDiscard $$Attr(nodiscard)

/** Tells the compiler the function is only an alias to another function **/
#define $$Alias(N) $$Attr(weak, alias(#N))

/** Declares to the compiler that the function should always be inlined **/
#define $$AlwaysInline $$Attr(always_inline)

/** The Base priority for Init and Finish functions **/
#define $BaseInitPriority$ 102

/**
 * @brief Similar to $$Constructor, runs before main however you can garantee it always runs
 * before any other $$Constructor
 **/
#define $$Init $$Attr(constructor($BaseInitPriority$)) void

/**
 * @brief Similar to $$Destructor, runs after main however you can garantee it always runs
 * after any other $$Destructor
 **/
#define $$Finish $$Attr(destructor($BaseInitPriority$)) void

/**
 * @brief Declares a finishing handler with a defined priority
 **/
#define $$FinishPrior(Prior) $$Attr(destructor($BaseInitPriority$ + Prior))

/**
 * @brief Declares an initializer handler with a defined priority
 **/
#define $$InitPrior(Prior) $$Attr(constructor($BaseInitPriority$ + Prior))


/**
 * @brief Declares an initializer handler that is garanted to run before any other
 * initializer
 **/
#define $$GlobalInit $$Attr(constructor($BaseInitPriority$-1))

/** Marks a function as not return point **/
#define $$NoReturn $$Attr(noreturn)
/** Attaches a cleanup function for a variable **/
#define $$Cleanup(F) $$Attr(cleanup(F))
/** Marks the variable as non initialized **/
#define $$NoInit $$Attr(noinit)

/** @brief Marks a type as a vector type of N fields **/
#define $$VecSize(T, N) $$Attr(vector_size(sizeof(T) * N))

/**
 * @brief Defines a new metadata section with type T
 * @param MetaName the metadata identifier
 * @param T the struct or type for the metadata
 **/
#define $DefineMeta(MetaName, T)\
  typedef T $MetaType$(MetaName);\
  extern const $MetaType$(MetaName) $$Attr(weak) $MetaStart$(MetaName)[];\
  extern const $MetaType$(MetaName) $$Attr(weak) $MetaEnd$(MetaName)[];

/**
 * @brief Gets the size in bytes of some metadata list
 * @param MetaName the metadata identifier 
 **/
#define $MetaSize$(MetaName) ($MetaEnd$(MetaName) - $MetaStart$(MetaName))

/**
 * @brief Macro to loop through each metadata entry
 * @param IterVariable the iterator variable identifier
 * @param MetaName the metadata identifier
 **/
#define $MetaForeach(IterVariable, MetaName)\
  int $Var$(it) = 0;\
  for (\
    const $MetaType$(MetaName) *$$Concat(IterVariable,_ptr) = $MetaStart$(MetaName);\
    ($Var$(it) = 0,$$Concat(IterVariable,_ptr) < $MetaEnd$(MetaName));\
    ++$$Concat(IterVariable,_ptr) )\
  for(const $MetaType$(MetaName) IterVariable = *$$Concat(NAME,_ptr);$Var$(it)==0;$Var$(it)++)

/**
 * @brief create a new metadata entry at compile time
 * @param MetaName the metadata identifier
 * @param Name the metadata entry name
 *
 * @note all metadata entries are constant and name would not be mangled, pointing directly to the metadata entry at runtime
 * So it is very usefull inside $MetaForeach to compare pointers of metadata entries
 *
 * @example 
 * #define $$CustomEndpointFunction(Route, Method, Name)\
 *  int $PVar$(Method, Name)(Context * ctx, Request $req, Response * $res);\
 *  $Meta(http_endpoint, $$Concat3(Meta, Method, Name), (CustomEndointMeta){.method = Method, .route = Route, .handler = $PVar$(Method, Name)});\
 *  int $PVar$(Method, Name)($$MaybeUnused Context * $ctx, $$MaybeUnused Request $req, $$MaybeUnused Response * $res)
 **/
#define $Meta(MetaName, Name, ...) $$Attr(used, section(#MetaName),aligned(8)) static const $MetaType$(MetaName) Name  = __VA_ARGS__

#endif //META_H_
