#ifndef __MOSDOP_H
#define __MOSDOP_H

#include "msg/Message.h"

/*
 * OSD op
 *
 * oid - object id
 * op  - OSD_OP_DELETE, etc.
 *
 */

#define OSD_OP_READ       1
#define OSD_OP_WRITE      2
#define OSD_OP_STAT       10
#define OSD_OP_DELETE     11
#define OSD_OP_TRUNCATE   12
#define OSD_OP_ZERORANGE  13
#define OSD_OP_MKFS       20

#define OSD_OP_FLAG_TRUNCATE  1   // truncate object after end of write

typedef struct {
  long tid;
  long pcid;
  msg_addr_t asker;

  object_t oid;
  repgroup_t rg;
  __uint64_t ocv;

  int op;
  size_t length, offset;

  size_t _data_len;
} MOSDOp_st;

class MOSDOp : public Message {
  MOSDOp_st st;
  bufferlist data;

  friend class MOSDOpReply;

 public:
  long get_tid() { return st.tid; }
  msg_addr_t get_asker() { return st.asker; }

  object_t get_oid() { return st.oid; }
  repgroup_t get_rg() { return st.rg; }
  __uint64_t get_ocv() { return st.ocv; }

  int get_op() { return st.op; }
  size_t get_length() { return st.length; }
  size_t get_offset() { return st.offset; }

  void set_data(bufferlist &d) {
	data.claim(d);
	st._data_len = data.length();
  }
  bufferlist& get_data() {
	return data;
  }
  size_t get_data_len() { return st._data_len; }

  // keep a pcid (procedure call id) to match up request+reply
  void set_pcid(long pcid) { this->st.pcid = pcid; }
  long get_pcid() { return st.pcid; }

  MOSDOp(long tid, msg_addr_t asker, 
		 object_t oid, repgroup_t rg, __uint64_t ocv, int op) :
	Message(MSG_OSD_OP) {
	memset(&st, 0, sizeof(st));
	this->st.tid = tid;
	this->st.asker = asker;

	this->st.oid = oid;
	this->st.rg = rg;
	this->st.ocv = ocv;
	this->st.op = op;
  }
  MOSDOp() {}

  void set_length(size_t l) { st.length = l; }
  void set_offset(size_t o) { st.offset = o; }

  
  // marshalling
  virtual void decode_payload() {
	payload.copy(0, sizeof(st), (char*)&st);
	payload.splice(0, sizeof(st));
	data.claim(payload);
  }
  virtual void encode_payload() {
	payload.push_back( new buffer((char*)&st, sizeof(st)) );
	payload.claim_append( data );
  }

  virtual char *get_type_name() { return "oop"; }
};

#endif
