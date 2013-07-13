#define PROPERTY_RO( TYPE, NAME, GETTER ) \
  public:                                 \
    TYPE GETTER() const { return NAME; }  \
  protected:                              \
    TYPE NAME;


#define PROPERTY_RW( TYPE, NAME, GETTER, SETTER ) \
  public:                                         \
    void SETTER( TYPE GETTER ) { NAME = GETTER; } \
  PROPERTY_RO( TYPE, NAME, GETTER )


#define PROPERTY_STATIC_RO( TYPE, NAME, GETTER ) \
  public:                                        \
    static TYPE GETTER() { return NAME; }        \
  protected:                                     \
    static TYPE NAME;


#define PROPERTY_STATIC_RW( TYPE, NAME, GETTER, SETTER ) \
  public:                                                \
    static void SETTER( TYPE GETTER ) { NAME = GETTER; } \
  PROPERTY_RO( TYPE, NAME, GETTER )


#define PROPERTY_MAKESLOTS( text )
// Usage:
//   PROPERTY_MAKESLOTS(
//     public slots:
//       ...
//     private:
//   )
