#ifndef _SQUIRREL_OBJECT_H_
#define _SQUIRREL_OBJECT_H_

class SquirrelObject
{
	friend class SquirrelVM;
public:
	SquirrelObject();
	virtual ~SquirrelObject();
	SquirrelObject(const SquirrelObject &o);
	SquirrelObject(HSQOBJECT o);
	SquirrelObject & operator =(const SquirrelObject &o);
	SquirrelObject & operator =(SQInteger n);
	void AttachToStackObject(SQInteger idx);
  void Reset(void); // Release (any) reference and reset _o.
  SquirrelObject Clone();
	BOOL SetValue(const SquirrelObject &key,const SquirrelObject &val);
	
	BOOL SetValue(SQInteger key,const SquirrelObject &val);
	BOOL SetValue(INT key,bool b); // Compiler treats SQBool as INT.
	BOOL SetValue(INT key,INT n);
	BOOL SetValue(INT key,FLOAT f);
	BOOL SetValue(INT key,const SQChar *s);

	BOOL SetValue(const SQChar *key,const SquirrelObject &val);
	BOOL SetValue(const SQChar *key,bool b);
	BOOL SetValue(const SQChar *key,INT n);
	BOOL SetValue(const SQChar *key,FLOAT f);
	BOOL SetValue(const SQChar *key,const SQChar *s);

  BOOL SetUserPointer(const SQChar * key,SQUserPointer up);
  SQUserPointer GetUserPointer(const SQChar * key);
  BOOL SetUserPointer(INT key,SQUserPointer up);
  SQUserPointer GetUserPointer(INT key);

  BOOL NewUserData(const SQChar * key,INT size,SQUserPointer * typetag=0);
  BOOL GetUserData(const SQChar * key,SQUserPointer * data,SQUserPointer * typetag=0);
  BOOL RawGetUserData(const SQChar * key,SQUserPointer * data,SQUserPointer * typetag=0);

  // === BEGIN Arrays ===

  BOOL ArrayResize(INT newSize);
  BOOL ArrayExtend(INT amount);
  BOOL ArrayReverse(void);
  SquirrelObject ArrayPop(SQBool returnPoppedVal=SQTrue);

  void ArrayAppend(const SquirrelObject &o);

  template<typename T>
  BOOL ArrayAppend(T item);

  // === END Arrays ===

	BOOL SetInstanceUP(SQUserPointer up);
	BOOL IsNull() const;
	SQInteger IsNumeric() const;
	SQInteger Len() const;
	BOOL SetDelegate(SquirrelObject &obj);
	SquirrelObject GetDelegate();
	const SQChar* ToString();
	bool ToBool();
	SQInteger ToInteger();
	SQFloat ToFloat();
	SQUserPointer GetInstanceUP(SQUserPointer tag) const;
	SquirrelObject GetValue(const SQChar *key) const;
	BOOL Exists(const SQChar *key) const;
	FLOAT GetFloat(const SQChar *key) const;
	INT GetInt(const SQChar *key) const;
	const SQChar *GetString(const SQChar *key) const;
	bool GetBool(const SQChar *key) const;
	SquirrelObject GetValue(INT key) const;
	FLOAT GetFloat(INT key) const;
	INT GetInt(INT key) const;
	const SQChar *GetString(INT key) const;
	bool GetBool(INT key) const;
	SquirrelObject GetAttributes(const SQChar *key = NULL);
	SQObjectType GetType();
	HSQOBJECT & GetObjectHandle(){return _o;}
	BOOL BeginIteration();
	BOOL Next(SquirrelObject &key,SquirrelObject &value);
	void EndIteration();

  BOOL GetTypeTag(SQUserPointer * typeTag);

  // === Get the type name of item/object through string key in a table or class. Returns NULL if the type name is not set (not an SqPlus registered type).
  const SQChar * GetTypeName(const SQChar * key);
  // === Get the type name of item/object through INT key in a table or class. Returns NULL if the type name is not set (not an SqPlus registered type).
  const SQChar * GetTypeName(INT key);
  // === Get the type name of this object, else return NULL if not an SqPlus registered type.
  const SQChar * GetTypeName(void);


private:
	BOOL GetSlot(const SQChar *name) const;
  BOOL RawGetSlot(const SQChar *name) const;
	BOOL GetSlot(INT key) const;
	HSQOBJECT _o;
};

struct StackHandler {
	StackHandler(HSQUIRRELVM v) {
		_top = sq_gettop(v);
		this->v = v;
	}
	SQFloat GetFloat(SQInteger idx) {
		SQFloat x = 0.0f;
		if(idx > 0 && idx <= _top) {
			sq_getfloat(v,idx,&x);
		}
		return x;
	}
	SQInteger GetInt(SQInteger idx) {
		SQInteger x = 0;
		if(idx > 0 && idx <= _top) {
			sq_getinteger(v,idx,&x);
		}
		return x;
	}
	HSQOBJECT GetObjectHandle(SQInteger idx) {
		HSQOBJECT x;
		if(idx > 0 && idx <= _top) {
			sq_resetobject(&x);
			sq_getstackobj(v,idx,&x);
		}
		return x;
	}
	const SQChar *GetString(SQInteger idx)
	{
    const SQChar *x = NULL;
		if(idx > 0 && idx <= _top) {
			sq_getstring(v,idx,&x);
		}
		return x;
	}
	SQUserPointer GetUserPointer(SQInteger idx)
	{
		SQUserPointer x = 0;
		if(idx > 0 && idx <= _top) {
			sq_getuserpointer(v,idx,&x);
		}
		return x;
	}
	SQUserPointer GetInstanceUp(SQInteger idx,SQUserPointer tag)
	{
		SQUserPointer self;
		if(SQ_FAILED(sq_getinstanceup(v,idx,(SQUserPointer*)&self,tag)))
			return NULL;
		return self;
	}
	SQUserPointer GetUserData(SQInteger idx,SQUserPointer tag=0)
	{
		SQUserPointer otag;
		SQUserPointer up;
		if(idx > 0 && idx <= _top) {
			if(SQ_SUCCEEDED(sq_getuserdata(v,idx,&up,&otag))) {
				if(tag == otag)
					return up;
			}
		}
		return NULL;
	}
	BOOL GetBool(SQInteger idx)
	{
		SQBool ret;
		if(idx > 0 && idx <= _top) {
			if(SQ_SUCCEEDED(sq_getbool(v,idx,&ret)))
				return ret;
		}
		return FALSE;
	}
	SQInteger GetType(SQInteger idx)
	{
		if(idx > 0 && idx <= _top) {
			return sq_gettype(v,idx);
		}
		return -1;
	}
	
	SQInteger GetParamCount() {
		return _top;
	}
	SQInteger Return(const SQChar *s)
	{
		sq_pushstring(v,s,-1);
		return 1;
	}
	SQInteger Return(FLOAT f)
	{
		sq_pushfloat(v,f);
		return 1;
	}
	SQInteger Return(INT i)
	{
		sq_pushinteger(v,i);
		return 1;
	}
	SQInteger Return(bool b)
	{
		sq_pushbool(v,b);
		return 1;
	}
  SQInteger Return(SQUserPointer p) {
    sq_pushuserpointer(v,p);
    return 1;
  }
	SQInteger Return(SquirrelObject &o)
	{
		sq_pushobject(v,o.GetObjectHandle());
		return 1;
	}
	SQInteger Return() { return 0; }
	SQInteger ThrowError(const SQChar *error) {
		return sq_throwerror(v,error);
	}
  HSQUIRRELVM GetVMPtr() { return v; }
private:
	SQInteger _top;
	HSQUIRRELVM v;
};

#endif //_SQUIRREL_OBJECT_H_
