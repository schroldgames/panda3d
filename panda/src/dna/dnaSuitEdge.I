// Filename: dnaSuitEdge.I
// Created by:  shochet (28Jan01)
//
////////////////////////////////////////////////////////////////////

INLINE bool DNASuitEdge::
operator == (const DNASuitEdge &other) const {
  return (_start_point == other._start_point &&
          _end_point == other._end_point);
}

#ifdef CPPPARSER
PT(DNASuitPoint) DNASuitEdge::get_start_point() const;
#else
INLINE PT(DNASuitPoint) DNASuitEdge::get_start_point() const {
  return _start_point;
}
#endif

#ifdef CPPPARSER
PT(DNASuitPoint) DNASuitEdge::get_end_point() const;
#else
INLINE PT(DNASuitPoint) DNASuitEdge::get_end_point() const {
  return _end_point;
}
#endif

INLINE std::string DNASuitEdge::get_zone_id() const {
  return _zone_id;
}

INLINE void DNASuitEdge::set_zone_id(std::string zone_id) {
  _zone_id = zone_id;
}
