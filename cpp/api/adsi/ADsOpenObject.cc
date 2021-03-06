
#include "../../misc.h"
#include "./IADs.h"
#include "./IADsContainer.h"
#include "./IDirectorySearch.h"

namespace myAddon {

class ADsOpenObjectWorker : public Napi::AsyncWorker {
 private:
  Napi::Promise::Deferred m_deferred;
  std::u16string m_binding;
  std::u16string m_user;
  std::u16string m_password;
  DWORD m_flag;
  IID m_riid;

  void *m_pObject;

 public:
  ADsOpenObjectWorker(Napi::Env &env, Napi::Promise::Deferred &deferred,
                      std::u16string &binding, std::u16string &user,
                      std::u16string &password, DWORD flag, IID riid)
      : AsyncWorker(env),
        m_deferred(deferred),
        m_binding(binding),
        m_user(user),
        m_password(password),
        m_flag(flag),
        m_riid(riid) {}

  ~ADsOpenObjectWorker() {}

  // This code will be executed on the worker thread
  void Execute() override {
    Napi::Env env = Env();

    LPCWSTR binding = (LPCWSTR)m_binding.c_str();

    LPCWSTR user = NULL;
    if (!m_user.empty()) {
      user = (LPCWSTR)m_user.c_str();
    }

    LPCWSTR password = NULL;
    if (!m_password.empty()) {
      password = (LPCWSTR)m_password.c_str();
    }

    HRESULT hr =
        ADsOpenObject(binding, user, password, m_flag, m_riid, &m_pObject);
    AD_CHECK_ERROR_ASYNC(hr, "ADsOpenObject");
  }

  void OnOK() override {
    Napi::Env env = Env();
    Napi::HandleScope scope(env);

    Napi::String s = Napi::String::New(env, p2s(m_pObject));
    Napi::Value result;

    if (m_riid == IID_IADsContainer) {
      result = E_IADsContainer::NewInstance(env, s);
    }
    if (m_riid == IID_IDirectorySearch) {
      result = E_IDirectorySearch::NewInstance(env, s);
    }
    if (m_riid == IID_IADs) {
      result = E_IADs::NewInstance(env, s);
    }
    m_deferred.Resolve(result);
  }

  void OnError(Napi::Error const &error) override {
    m_deferred.Reject(error.Value());
  }
};

Napi::Value e_ADsOpenObject(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  auto deferred = Napi::Promise::Deferred::New(env);
  CHECK_INPUT_DEFERRED(
      "ADsOpenObject({\n"
      "bindingUri: string,\n"
      "user?: string,\n"
      "password?: string\n"
      "adsAuthentication?: ADS_AUTHENTICATION_ENUM,\n"
      "riid?: IIDClass\n"
      "})",
      1);

  Napi::Object input = info[0].As<Napi::Object>();

  std::u16string binding = input.Get("binding").As<Napi::String>().Utf16Value();

  std::u16string user;
  if (input.Has("user")) {
    std::u16string user = input.Get("user").As<Napi::String>().Utf16Value();
  }

  std::u16string password;
  if (input.Has("password")) {
    std::u16string password =
        input.Get("password").As<Napi::String>().Utf16Value();
  }

  DWORD flag = getFlag(env, ADS_AUTHENTICATION_FLAGS, input,
                       "authenticationFlag", ADS_SECURE_AUTHENTICATION);

  IID riid = IID_IADs;
  if (input.Has("riid")) {
    std::string outputTypeStr =
        input.Get("riid").As<Napi::String>().Utf8Value();
    if (outputTypeStr == "IID_IADsContainer") {
      riid = IID_IADsContainer;
    }
    if (outputTypeStr == "IID_IDirectorySearch") {
      riid = IID_IDirectorySearch;
    }
  }

  ADsOpenObjectWorker *w = new ADsOpenObjectWorker(env, deferred, binding, user,
                                                   password, flag, riid);
  w->Queue();
  return deferred.Promise();
}

Napi::Value e_ADsOpenObjectSync(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  CHECK_INPUT(
      "ADsOpenObject({\n"
      "bindingUri: string,\n"
      "user?: string,\n"
      "password?: string\n"
      "adsAuthentication?: ADS_AUTHENTICATION_ENUM,\n"
      "riid?: IIDClass\n"
      "})",
      1);

  Napi::Object input = info[0].As<Napi::Object>();

  std::u16string binding = input.Get("binding").As<Napi::String>().Utf16Value();

  std::u16string user;
  if (input.Has("user")) {
    std::u16string user = input.Get("user").As<Napi::String>().Utf16Value();
  }

  std::u16string password;
  if (input.Has("password")) {
    std::u16string password =
        input.Get("password").As<Napi::String>().Utf16Value();
  }

  DWORD flag = getFlag(env, ADS_AUTHENTICATION_FLAGS, input,
                       "authenticationFlag", ADS_SECURE_AUTHENTICATION);

  IID riid = IID_IADs;
  if (input.Has("riid")) {
    std::string outputTypeStr =
        input.Get("riid").As<Napi::String>().Utf8Value();
    if (outputTypeStr == "IID_IADsContainer") {
      riid = IID_IADsContainer;
    }
    if (outputTypeStr == "IID_IDirectorySearch") {
      riid = IID_IDirectorySearch;
    }
  }

  LPCWSTR pBinding = (LPCWSTR)binding.c_str();

  LPCWSTR pUser = NULL;
  if (!user.empty()) {
    pUser = (LPCWSTR)user.c_str();
  }

  LPCWSTR pPassword = NULL;
  if (!password.empty()) {
    pPassword = (LPCWSTR)password.c_str();
  }

  void *pObject;

  HRESULT hr = ADsOpenObject(pBinding, pUser, pPassword, flag, riid, &pObject);
  AD_CHECK_ERROR(hr, "ADsOpenObject");

  Napi::String s = Napi::String::New(env, p2s(pObject));
  Napi::Value result;

  if (riid == IID_IADsContainer) {
    result = E_IADsContainer::NewInstance(env, s);
  }
  if (riid == IID_IDirectorySearch) {
    result = E_IDirectorySearch::NewInstance(env, s);
  }
  if (riid == IID_IADs) {
    result = E_IADs::NewInstance(env, s);
  }
  return result;
}

}  // namespace myAddon
