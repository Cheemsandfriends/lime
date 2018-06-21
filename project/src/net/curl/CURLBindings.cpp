#include <curl/curl.h>
#include <system/CFFI.h>
#include <system/CFFIPointer.h>
#include <system/Mutex.h>
#include <system/System.h>
#include <system/ValuePointer.h>
#include <utils/Bytes.h>
#include <string.h>
#include <map>
#include <sstream>
#include <vector>


namespace lime {
	
	
	struct CURL_Progress {
		
		double dltotal;
		double dlnow;
		double ultotal;
		double ulnow;
		
	};
	
	struct CURL_XferInfo {
		
		curl_off_t dltotal;
		curl_off_t dlnow;
		curl_off_t ultotal;
		curl_off_t ulnow;
		
	};
	
	std::map<void*, CURLM*> curlMultiHandles;
	std::map<void*, int> curlMultiRunningHandles;
	std::map<void*, bool> curlMultiValid;
	std::map<CURL*, void*> curlObjects;
	std::map<void*, bool> curlValid;
	std::map<void*, ValuePointer*> headerCallbacks;
	std::map<void*, curl_slist*> headerSLists;
	std::map<void*, std::vector<char*>* > headerValues;
	std::map<void*, ValuePointer*> progressCallbacks;
	std::map<void*, CURL_Progress*> progressValues;
	std::map<void*, Bytes*> readBytes;
	std::map<void*, int> readBytesPosition;
	std::map<void*, ValuePointer*> readBytesRoot;
	std::map<void*, std::stringbuf*> writeBuffers;
	std::map<void*, Bytes*> writeBytes;
	std::map<void*, ValuePointer*> writeBytesRoot;
	std::map<void*, ValuePointer*> writeCallbacks;
	std::map<void*, ValuePointer*> xferInfoCallbacks;
	std::map<void*, CURL_XferInfo*> xferInfoValues;
	Mutex curl_gc_mutex;
	
	
	void gc_curl (value handle) {
		
		if (!val_is_null (handle)) {
			
			curl_gc_mutex.Lock ();
			
			if (curlMultiHandles.find (handle) != curlMultiHandles.end ()) {
				
				CURLM* multi = curlMultiHandles[handle];
				curl_multi_remove_handle (multi, handle);
				curlMultiHandles.erase (handle);
				
			}
			
			if (curlValid.find (handle) != curlValid.end ()) {
				
				CURL* curl = (CURL*)val_data (handle);
				
				curlValid.erase (handle);
				curlObjects.erase (curl);
				curl_easy_cleanup (curl);
				
			}
			
			ValuePointer* callback;
			Bytes* bytes;
			ValuePointer* bytesRoot;
			
			if (headerCallbacks.find (handle) != headerCallbacks.end ()) {
				
				callback = headerCallbacks[handle];
				std::vector<char*>* values = headerValues[handle];
				headerCallbacks.erase (handle);
				headerValues.erase (handle);
				delete callback;
				delete values;
				
			}
			
			if (headerSLists.find (handle) != headerSLists.end ()) {
				
				curl_slist* chunk = headerSLists[handle];
				headerSLists.erase (handle);
				curl_slist_free_all (chunk);
				
			}
			
			if (progressCallbacks.find (handle) != progressCallbacks.end ()) {
				
				callback = progressCallbacks[handle];
				CURL_Progress* progress = progressValues[handle];
				progressCallbacks.erase (handle);
				progressValues.erase (progress);
				delete callback;
				delete progress;
				
			}
			
			if (readBytes.find (handle) != readBytes.end ()) {
				
				bytes = readBytes[handle];
				bytesRoot = readBytesRoot[handle];
				readBytes.erase (handle);
				readBytesPosition.erase (handle);
				readBytesRoot.erase (handle);
				delete bytes;
				delete bytesRoot;
				
			}
			
			if (writeCallbacks.find (handle) != writeCallbacks.end ()) {
				
				callback = writeCallbacks[handle];
				bytes = writeBytes[handle];
				bytesRoot = writeBytesRoot[handle];
				writeCallbacks.erase (handle);
				writeBytes.erase (handle);
				writeBytesRoot.erase (handle);
				delete callback;
				delete bytes;
				delete bytesRoot;
				
			}
			
			if (xferInfoCallbacks.find (handle) != xferInfoCallbacks.end ()) {
				
				callback = xferInfoCallbacks[handle];
				CURL_XferInfo* info = xferInfoValues[handle];
				xferInfoCallbacks.erase (handle);
				xferInfoValues.erase (handle);
				delete callback;
				delete info;
				
			}
			
			val_gc (handle, 0);
			
			curl_gc_mutex.Unlock ();
			
		}
		
	}
	
	
	void hl_gc_curl (HL_CFFIPointer* handle) {
		
		if (handle) {
			
			curl_gc_mutex.Lock ();
			
			if (curlMultiHandles.find (handle) != curlMultiHandles.end ()) {
				
				CURLM* multi = curlMultiHandles[handle];
				curl_multi_remove_handle (multi, handle);
				curlMultiHandles.erase (handle);
				
			}
			
			if (curlValid.find (handle) != curlValid.end ()) {
				
				CURL* curl = (CURL*)handle->ptr;
				
				curlValid.erase (handle);
				curlObjects.erase (curl);
				curl_easy_cleanup (curl);
				
			}
			
			ValuePointer* callback;
			Bytes* bytes;
			ValuePointer* bytesRoot;
			
			if (headerCallbacks.find (handle) != headerCallbacks.end ()) {
				
				callback = headerCallbacks[handle];
				std::vector<char*>* values = headerValues[handle];
				headerCallbacks.erase (handle);
				headerValues.erase (handle);
				delete callback;
				delete values;
				
			}
			
			if (headerSLists.find (handle) != headerSLists.end ()) {
				
				curl_slist* chunk = headerSLists[handle];
				headerSLists.erase (handle);
				curl_slist_free_all (chunk);
				
			}
			
			if (progressCallbacks.find (handle) != progressCallbacks.end ()) {
				
				callback = progressCallbacks[handle];
				CURL_Progress* progress = progressValues[handle];
				progressCallbacks.erase (handle);
				progressValues.erase (progress);
				delete callback;
				delete progress;
				
			}
			
			if (readBytes.find (handle) != readBytes.end ()) {
				
				bytes = readBytes[handle];
				bytesRoot = readBytesRoot[handle];
				readBytes.erase (handle);
				readBytesPosition.erase (handle);
				readBytesRoot.erase (handle);
				delete bytesRoot;
				
			}
			
			if (writeCallbacks.find (handle) != writeCallbacks.end ()) {
				
				callback = writeCallbacks[handle];
				bytes = writeBytes[handle];
				bytesRoot = writeBytesRoot[handle];
				writeCallbacks.erase (handle);
				writeBytes.erase (handle);
				writeBytesRoot.erase (handle);
				delete callback;
				delete bytesRoot;
				
			}
			
			if (xferInfoCallbacks.find (handle) != xferInfoCallbacks.end ()) {
				
				callback = xferInfoCallbacks[handle];
				CURL_XferInfo* info = xferInfoValues[handle];
				xferInfoCallbacks.erase (handle);
				xferInfoValues.erase (handle);
				delete callback;
				delete info;
				
			}
			
			handle->finalizer = NULL;
			
			curl_gc_mutex.Unlock ();
			
		}
		
	}
	
	
	void gc_curl_multi (value handle) {
		
		if (!val_is_null (handle)) {
			
			curl_gc_mutex.Lock ();
			
			if (curlMultiValid.find (handle) != curlMultiValid.end ()) {
				
				curlMultiValid.erase (handle);
				curl_multi_cleanup ((CURL*)val_data(handle));
				
			}
			
			for (std::map<void*, void*>::iterator it = curlMultiHandles.begin (); it != curlMultiHandles.end (); ++it) {
				
				if (curlMultiHandles[it->first] == handle) {
					
					gc_curl ((value)it->first);
					
				}
				
			}
			
			val_gc (handle, 0);
			//handle = alloc_null ();
			
			curl_gc_mutex.Unlock ();
			
		}
		
	}
	
	
	void hl_gc_curl_multi (HL_CFFIPointer* handle) {
		
		if (handle) {
			
			curl_gc_mutex.Lock ();
			
			if (curlMultiValid.find (handle) != curlMultiValid.end ()) {
				
				curlMultiValid.erase (handle);
				curl_multi_cleanup ((CURL*)handle->ptr);
				
			}
			
			for (std::map<void*, void*>::iterator it = curlMultiHandles.begin (); it != curlMultiHandles.end (); ++it) {
				
				if (curlMultiHandles[it->first] == handle) {
					
					hl_gc_curl ((HL_CFFIPointer*)it->first);
					
				}
				
			}
			
			handle->finalizer = NULL;
			//handle = alloc_null ();
			
			curl_gc_mutex.Unlock ();
			
		}
		
	}
	
	
	void lime_curl_easy_cleanup (value handle) {
		
		gc_curl (handle);
		
	}
	
	
	HL_PRIM void hl_lime_curl_easy_cleanup (HL_CFFIPointer* handle) {
		
		hl_gc_curl (handle);
		
	}
	
	
	value lime_curl_easy_duphandle (value handle) {
		
		curl_gc_mutex.Lock ();
		
		CURL* dup = curl_easy_duphandle ((CURL*)val_data(handle));
		value duphandle = CFFIPointer (dup, gc_curl);
		curlValid[duphandle] = true;
		curlObjects[dup] = duphandle;
		
		value callbackValue;
		Bytes* bytes;
		value bytesValue;
		
		if (headerCallbacks.find (handle) != headerCallbacks.end ()) {
			
			callbackValue = (value)headerCallbacks[handle]->Get ();
			headerCallbacks[duphandle] = new ValuePointer (callbackValue);
			headerValues[duphandle] = new std::vector<char*> ();
			
		}
		
		if (progressCallbacks.find (handle) != progressCallbacks.end ()) {
			
			callbackValue = (value)progressCallbacks[handle]->Get ();
			progressCallbacks[duphandle] = new ValuePointer (callbackValue);
			progressValues[duphandle] = new CURL_Progress ();
			
		}
		
		if (readBytes.find (handle) != readBytes.end ()) {
			
			bytesValue = (value)readBytesRoot[handle]->Get ();
			bytes = new Bytes (bytesValue);
			readBytes[duphandle] = bytes;
			readBytesPosition[duphandle] = 0;
			readBytesRoot[duphandle] = new ValuePointer (bytesValue);
			
		}
		
		if (writeCallbacks.find (handle) != writeCallbacks.end ()) {
			
			callbackValue = (value)writeCallbacks[handle]->Get ();
			bytesValue = (value)writeBytesRoot[handle]->Get ();
			bytes = new Bytes (bytesValue);
			writeCallbacks[duphandle] = new ValuePointer (callbackValue);
			writeBuffers[duphandle] = new std::stringbuf ();
			writeBytes[duphandle] = bytes;
			writeBytesRoot[duphandle] = new ValuePointer (bytesValue);
			
		}
		
		if (xferInfoCallbacks.find (handle) != xferInfoCallbacks.end ()) {
			
			callbackValue = (value)xferInfoCallbacks[handle]->Get ();
			xferInfoCallbacks[duphandle] = new ValuePointer (callbackValue);
			xferInfoValues[duphandle] = new CURL_XferInfo ();
			
		}
		
		curl_gc_mutex.Unlock ();
		
		return duphandle;
		
	}
	
	
	HL_PRIM HL_CFFIPointer* hl_lime_curl_easy_duphandle (HL_CFFIPointer* handle) {
		
		curl_gc_mutex.Lock ();
		
		CURL* dup = curl_easy_duphandle ((CURL*)handle->ptr);
		HL_CFFIPointer* duphandle = HLCFFIPointer (dup, (hl_finalizer)hl_gc_curl);
		curlValid[duphandle] = true;
		curlObjects[dup] = duphandle;
		
		vclosure* callbackValue;
		Bytes* bytes;
		
		if (headerCallbacks.find (handle) != headerCallbacks.end ()) {
			
			callbackValue = (vclosure*)headerCallbacks[handle]->Get ();
			headerCallbacks[duphandle] = new ValuePointer (callbackValue);
			headerValues[duphandle] = new std::vector<char*> ();
			
		}
		
		if (progressCallbacks.find (handle) != progressCallbacks.end ()) {
			
			callbackValue = (vclosure*)progressCallbacks[handle]->Get ();
			progressCallbacks[duphandle] = new ValuePointer (callbackValue);
			progressValues[duphandle] = new CURL_Progress ();
			
		}
		
		if (readBytes.find (handle) != readBytes.end ()) {
			
			readBytes[duphandle] = readBytes[handle];
			readBytesPosition[duphandle] = 0;
			readBytesRoot[duphandle] = new ValuePointer ((vobj*)readBytes[handle]);
			
		}
		
		if (writeCallbacks.find (handle) != writeCallbacks.end ()) {
			
			callbackValue = (vclosure*)writeCallbacks[handle]->Get ();
			writeCallbacks[duphandle] = new ValuePointer (callbackValue);
			writeBuffers[duphandle] = new std::stringbuf ();
			writeBytes[duphandle] = writeBytes[handle];
			writeBytesRoot[duphandle] = new ValuePointer ((vobj*)writeBytes[handle]);
			
		}
		
		if (xferInfoCallbacks.find (handle) != xferInfoCallbacks.end ()) {
			
			callbackValue = (vclosure*)xferInfoCallbacks[handle]->Get ();
			xferInfoCallbacks[duphandle] = new ValuePointer (callbackValue);
			xferInfoValues[duphandle] = new CURL_XferInfo ();
			
		}
		
		curl_gc_mutex.Unlock ();
		
		return duphandle;
		
	}
	
	
	value lime_curl_easy_escape (value curl, HxString url, int length) {
		
		char* result = curl_easy_escape ((CURL*)val_data(curl), url.__s, length);
		return result ? alloc_string (result) : alloc_null ();
		
	}
	
	
	HL_PRIM vbyte* hl_lime_curl_easy_escape (HL_CFFIPointer* curl, hl_vstring* url, int length) {
		
		char* result = curl_easy_escape ((CURL*)curl->ptr, url ? hl_to_utf8 (url->bytes) : NULL, length);
		return (vbyte*)result;
		
	}
	
	
	void lime_curl_easy_flush (value easy_handle) {
		
		curl_gc_mutex.Lock ();
		int code;
		
		if (headerCallbacks.find (easy_handle) != headerCallbacks.end ()) {
			
			ValuePointer* headerCallback = headerCallbacks[easy_handle];
			std::vector<char*>* values = headerValues[easy_handle];
			
			if (values->size () > 0) {
				
				for (std::vector<char*>::iterator it = values->begin (); it != values->end (); ++it) {
					
					headerCallback->Call (alloc_string (*it));
					
				}
				
				values->clear ();
				
			}
			
		}
		
		if (writeBuffers.find (easy_handle) != writeBuffers.end ()) {
			
			std::stringbuf* buffer = writeBuffers[easy_handle];
			int length = buffer->in_avail ();
			
			if (length > 0) {
				
				if (writeCallbacks.find (easy_handle) != writeCallbacks.end ()) {
					
					ValuePointer* writeCallback = writeCallbacks[easy_handle];
					ValuePointer* bytesRoot = writeBytesRoot[easy_handle];
					
					Bytes* bytes = writeBytes[easy_handle];
					bytes->Resize (length);
					buffer->sputn ((char*)bytes->b, length);
					buffer->str ("");
					
					length = val_int ((value)writeCallback->Call (bytes->Value ((value)bytesRoot->Get ())));
					
					if (length == CURL_WRITEFUNC_PAUSE) {
						
						// TODO: Handle pause
						
					}
					
				}
				
			}
			
		}
		
		if (progressCallbacks.find (easy_handle) != progressCallbacks.end ()) {
			
			CURL_Progress* progress = progressValues[easy_handle];
			ValuePointer* progressCallback = progressCallbacks[easy_handle];
			
			code = val_int ((value)progressCallback->Call (alloc_float (progress->dltotal), alloc_float (progress->dlnow), alloc_float (progress->ultotal), alloc_float (progress->ulnow)));
			
			if (code != 0) { // CURLE_OK
				
				// TODO: Abort
				
			}
			
		}
		
		if (xferInfoCallbacks.find (easy_handle) != xferInfoCallbacks.end ()) {
			
			CURL_XferInfo* xferInfo = xferInfoValues[easy_handle];
			ValuePointer* xferInfoCallback = xferInfoCallbacks[easy_handle];
			
			code = val_int ((value)xferInfoCallback->Call (alloc_int (xferInfo->dltotal), alloc_int (xferInfo->dlnow), alloc_int (xferInfo->ultotal), alloc_int (xferInfo->ulnow)));
			
			if (code != 0) {
				
				// TODO: Abort
				
			}
			
		}
		
	}
	
	
	HL_PRIM void hl_lime_curl_easy_flush (HL_CFFIPointer* easy_handle) {
		
		curl_gc_mutex.Lock ();
		int code;
		
		if (headerCallbacks.find (easy_handle) != headerCallbacks.end ()) {
			
			ValuePointer* headerCallback = headerCallbacks[easy_handle];
			std::vector<char*>* values = headerValues[easy_handle];
			
			if (values->size () > 0) {
				
				for (std::vector<char*>::iterator it = values->begin (); it != values->end (); ++it) {
					
					vdynamic* bytes = hl_alloc_dynamic (&hlt_bytes);
					bytes->v.bytes = (vbyte*)*it;
					
					headerCallback->Call (bytes);
					
				}
				
				values->clear ();
				
			}
			
		}
		
		if (writeBuffers.find (easy_handle) != writeBuffers.end ()) {
			
			std::stringbuf* buffer = writeBuffers[easy_handle];
			int length = buffer->in_avail ();
			
			if (length > 0) {
				
				if (writeCallbacks.find (easy_handle) != writeCallbacks.end ()) {
					
					ValuePointer* writeCallback = writeCallbacks[easy_handle];
					
					Bytes* bytes = writeBytes[easy_handle];
					bytes->Resize (length);
					buffer->sputn ((char*)bytes->b, length);
					buffer->str ("");
					
					length = (int)writeCallback->Call (bytes);
					
					if (length == CURL_WRITEFUNC_PAUSE) {
						
						// TODO: Handle pause
						
					}
					
				}
				
			}
			
		}
		
		if (progressCallbacks.find (easy_handle) != progressCallbacks.end ()) {
			
			CURL_Progress* progress = progressValues[easy_handle];
			ValuePointer* progressCallback = progressCallbacks[easy_handle];
			
			vdynamic* dltotal = hl_alloc_dynamic (&hlt_f64);
			vdynamic* dlnow = hl_alloc_dynamic (&hlt_f64);
			vdynamic* ultotal = hl_alloc_dynamic (&hlt_f64);
			vdynamic* ulnow = hl_alloc_dynamic (&hlt_f64);
			
			dltotal->v.d = progress->dltotal;
			dlnow->v.d = progress->dlnow;
			ultotal->v.d = progress->ultotal;
			ulnow->v.d = progress->ulnow;
			
			code = (int)progressCallback->Call (dltotal, dlnow, ultotal, ulnow);
			
			if (code != 0) { // CURLE_OK
				
				// TODO: Abort
				
			}
			
		}
		
		if (xferInfoCallbacks.find (easy_handle) != xferInfoCallbacks.end ()) {
			
			CURL_XferInfo* xferInfo = xferInfoValues[easy_handle];
			ValuePointer* xferInfoCallback = xferInfoCallbacks[easy_handle];
			
			vdynamic* dltotal = hl_alloc_dynamic (&hlt_i32);
			vdynamic* dlnow = hl_alloc_dynamic (&hlt_i32);
			vdynamic* ultotal = hl_alloc_dynamic (&hlt_i32);
			vdynamic* ulnow = hl_alloc_dynamic (&hlt_i32);
			
			dltotal->v.i = xferInfo->dltotal;
			dlnow->v.i = xferInfo->dlnow;
			ultotal->v.i = xferInfo->ultotal;
			ulnow->v.i = xferInfo->ulnow;
			
			code = (int)xferInfoCallback->Call (dltotal, dlnow, ultotal, ulnow);
			
			if (code != 0) {
				
				// TODO: Abort
				
			}
			
		}
		
	}
	
	
	value lime_curl_easy_getinfo (value curl, int info) {
		
		CURLcode code = CURLE_OK;
		CURL* handle = (CURL*)val_data(curl);
		CURLINFO type = (CURLINFO)info;
		
		switch (type) {
			
			case CURLINFO_EFFECTIVE_URL:
			case CURLINFO_REDIRECT_URL:
			case CURLINFO_CONTENT_TYPE:
			case CURLINFO_PRIVATE:
			case CURLINFO_PRIMARY_IP:
			case CURLINFO_LOCAL_IP:
			case CURLINFO_FTP_ENTRY_PATH:
			case CURLINFO_RTSP_SESSION_ID:
			case CURLINFO_SCHEME:
				
				char stringValue;
				code = curl_easy_getinfo (handle, type, &stringValue);
				return alloc_string (&stringValue);
				break;
			
			case CURLINFO_RESPONSE_CODE:
			case CURLINFO_HTTP_CONNECTCODE:
			case CURLINFO_FILETIME:
			case CURLINFO_REDIRECT_COUNT:
			case CURLINFO_HEADER_SIZE:
			case CURLINFO_REQUEST_SIZE:
			case CURLINFO_SSL_VERIFYRESULT:
			case CURLINFO_HTTPAUTH_AVAIL:
			case CURLINFO_PROXYAUTH_AVAIL:
			case CURLINFO_OS_ERRNO:
			case CURLINFO_NUM_CONNECTS:
			case CURLINFO_PRIMARY_PORT:
			case CURLINFO_LOCAL_PORT:
			case CURLINFO_LASTSOCKET:
			case CURLINFO_CONDITION_UNMET:
			case CURLINFO_RTSP_CLIENT_CSEQ:
			case CURLINFO_RTSP_SERVER_CSEQ:
			case CURLINFO_RTSP_CSEQ_RECV:
			case CURLINFO_HTTP_VERSION:
			case CURLINFO_PROXY_SSL_VERIFYRESULT:
			case CURLINFO_PROTOCOL:
			case CURLINFO_SIZE_UPLOAD_T: // TODO: These should be larger
			case CURLINFO_SIZE_DOWNLOAD_T:
			case CURLINFO_SPEED_DOWNLOAD_T:
			case CURLINFO_SPEED_UPLOAD_T:
			case CURLINFO_CONTENT_LENGTH_DOWNLOAD_T:
			case CURLINFO_CONTENT_LENGTH_UPLOAD_T:
				
				long intValue;
				code = curl_easy_getinfo (handle, type, &intValue);
				return alloc_int (intValue);
				break;
			
			case CURLINFO_TOTAL_TIME:
			case CURLINFO_NAMELOOKUP_TIME:
			case CURLINFO_CONNECT_TIME:
			case CURLINFO_APPCONNECT_TIME:
			case CURLINFO_PRETRANSFER_TIME:
			case CURLINFO_STARTTRANSFER_TIME:
			case CURLINFO_REDIRECT_TIME:
			case CURLINFO_SIZE_UPLOAD:
			case CURLINFO_SIZE_DOWNLOAD:
			case CURLINFO_SPEED_DOWNLOAD:
			case CURLINFO_SPEED_UPLOAD:
			case CURLINFO_CONTENT_LENGTH_DOWNLOAD:
			case CURLINFO_CONTENT_LENGTH_UPLOAD:
				
				double floatValue;
				code = curl_easy_getinfo (handle, type, &floatValue);
				return alloc_float (floatValue);
				break;
			
			case CURLINFO_SSL_ENGINES:
			case CURLINFO_COOKIELIST:
			case CURLINFO_CERTINFO:
			case CURLINFO_TLS_SESSION:
			case CURLINFO_TLS_SSL_PTR:
			case CURLINFO_ACTIVESOCKET:
				
				// TODO
				
				break;
			
			case CURLINFO_NONE:
			case CURLINFO_LASTONE:
				
				// ignore
				
				break;
			
			
		}
		
		return alloc_null ();
		
	}
	
	
	HL_PRIM vdynamic* hl_lime_curl_easy_getinfo (HL_CFFIPointer* curl, int info) {
		
		CURLcode code = CURLE_OK;
		CURL* handle = (CURL*)curl->ptr;
		CURLINFO type = (CURLINFO)info;
		
		int size;
		vdynamic* result = NULL;
		
		switch (type) {
			
			case CURLINFO_EFFECTIVE_URL:
			case CURLINFO_REDIRECT_URL:
			case CURLINFO_CONTENT_TYPE:
			case CURLINFO_PRIVATE:
			case CURLINFO_PRIMARY_IP:
			case CURLINFO_LOCAL_IP:
			case CURLINFO_FTP_ENTRY_PATH:
			case CURLINFO_RTSP_SESSION_ID:
			case CURLINFO_SCHEME:
			{
				char stringValue;
				code = curl_easy_getinfo (handle, type, &stringValue);
				
				int size = strlen (&stringValue) + 1;
				char* val = (char*)malloc (size);
				memcpy (val, &stringValue, size);
				
				result = hl_alloc_dynamic (&hlt_bytes);
				result->v.b = val;
				return result;
				break;
			}
			
			case CURLINFO_RESPONSE_CODE:
			case CURLINFO_HTTP_CONNECTCODE:
			case CURLINFO_FILETIME:
			case CURLINFO_REDIRECT_COUNT:
			case CURLINFO_HEADER_SIZE:
			case CURLINFO_REQUEST_SIZE:
			case CURLINFO_SSL_VERIFYRESULT:
			case CURLINFO_HTTPAUTH_AVAIL:
			case CURLINFO_PROXYAUTH_AVAIL:
			case CURLINFO_OS_ERRNO:
			case CURLINFO_NUM_CONNECTS:
			case CURLINFO_PRIMARY_PORT:
			case CURLINFO_LOCAL_PORT:
			case CURLINFO_LASTSOCKET:
			case CURLINFO_CONDITION_UNMET:
			case CURLINFO_RTSP_CLIENT_CSEQ:
			case CURLINFO_RTSP_SERVER_CSEQ:
			case CURLINFO_RTSP_CSEQ_RECV:
			case CURLINFO_HTTP_VERSION:
			case CURLINFO_PROXY_SSL_VERIFYRESULT:
			case CURLINFO_PROTOCOL:
			case CURLINFO_SIZE_UPLOAD_T: // TODO: These should be larger
			case CURLINFO_SIZE_DOWNLOAD_T:
			case CURLINFO_SPEED_DOWNLOAD_T:
			case CURLINFO_SPEED_UPLOAD_T:
			case CURLINFO_CONTENT_LENGTH_DOWNLOAD_T:
			case CURLINFO_CONTENT_LENGTH_UPLOAD_T:
			{
				long intValue;
				code = curl_easy_getinfo (handle, type, &intValue);
				
				result = hl_alloc_dynamic (&hlt_i32);
				result->v.i = intValue;
				return result;
				break;
			}
			
			case CURLINFO_TOTAL_TIME:
			case CURLINFO_NAMELOOKUP_TIME:
			case CURLINFO_CONNECT_TIME:
			case CURLINFO_APPCONNECT_TIME:
			case CURLINFO_PRETRANSFER_TIME:
			case CURLINFO_STARTTRANSFER_TIME:
			case CURLINFO_REDIRECT_TIME:
			case CURLINFO_SIZE_UPLOAD:
			case CURLINFO_SIZE_DOWNLOAD:
			case CURLINFO_SPEED_DOWNLOAD:
			case CURLINFO_SPEED_UPLOAD:
			case CURLINFO_CONTENT_LENGTH_DOWNLOAD:
			case CURLINFO_CONTENT_LENGTH_UPLOAD:
			{
				double floatValue;
				code = curl_easy_getinfo (handle, type, &floatValue);
				
				result = hl_alloc_dynamic (&hlt_f64);
				result->v.d = floatValue;
				return result;
				break;
			}
			
			case CURLINFO_SSL_ENGINES:
			case CURLINFO_COOKIELIST:
			case CURLINFO_CERTINFO:
			case CURLINFO_TLS_SESSION:
			case CURLINFO_TLS_SSL_PTR:
			case CURLINFO_ACTIVESOCKET:
				
				// TODO
				
				break;
			
			case CURLINFO_NONE:
			case CURLINFO_LASTONE:
				
				// ignore
				
				break;
			
			
		}
		
		return NULL;
		
	}
	
	
	value lime_curl_easy_init () {
		
		curl_gc_mutex.Lock ();
		
		CURL* curl = curl_easy_init ();
		value handle = CFFIPointer (curl, gc_curl);
		
		if (curlValid.find (handle) != curlValid.end ()) {
			
			printf ("Error: Duplicate cURL handle\n");
			
		}
		
		if (headerCallbacks.find (handle) != headerCallbacks.end ()) {
			
			printf ("Error: cURL handle already has a header callback\n");
			
		}
		
		if (progressCallbacks.find (handle) != progressCallbacks.end ()) {
			
			printf ("Error: cURL handle already has a progress callback\n");
			
		}
		
		if (readBytes.find (handle) != readBytes.end ()) {
			
			printf ("Error: cURL handle already has a read data value\n");
			
		}
		
		if (writeCallbacks.find (handle) != writeCallbacks.end ()) {
			
			printf ("Error: cURL handle already has a write callback\n");
			
		}
		
		curlValid[handle] = true;
		curlObjects[curl] = handle;
		
		writeBuffers[handle] = new std::stringbuf ();
		
		curl_gc_mutex.Unlock ();
		
		return handle;
		
	}
	
	
	HL_PRIM HL_CFFIPointer* hl_lime_curl_easy_init () {
		
		curl_gc_mutex.Lock ();
		
		CURL* curl = curl_easy_init ();
		HL_CFFIPointer* handle = HLCFFIPointer (curl, (hl_finalizer)hl_gc_curl);
		
		if (curlValid.find (handle) != curlValid.end ()) {
			
			printf ("Error: Duplicate cURL handle\n");
			
		}
		
		if (headerCallbacks.find (handle) != headerCallbacks.end ()) {
			
			printf ("Error: cURL handle already has a header callback\n");
			
		}
		
		if (progressCallbacks.find (handle) != progressCallbacks.end ()) {
			
			printf ("Error: cURL handle already has a progress callback\n");
			
		}
		
		if (readBytes.find (handle) != readBytes.end ()) {
			
			printf ("Error: cURL handle already has a read data value\n");
			
		}
		
		if (writeCallbacks.find (handle) != writeCallbacks.end ()) {
			
			printf ("Error: cURL handle already has a write callback\n");
			
		}
		
		curlValid[handle] = true;
		curlObjects[curl] = handle;
		
		writeBuffers[handle] = new std::stringbuf ();
		
		curl_gc_mutex.Unlock ();
		
		return handle;
		
	}
	
	
	int lime_curl_easy_pause (value handle, int bitmask) {
		
		return curl_easy_pause ((CURL*)val_data(handle), bitmask);
		
	}
	
	
	HL_PRIM int hl_lime_curl_easy_pause (HL_CFFIPointer* handle, int bitmask) {
		
		return curl_easy_pause ((CURL*)handle->ptr, bitmask);
		
	}
	
	
	int lime_curl_easy_perform (value easy_handle) {
		
		int code;
		System::GCEnterBlocking ();
		
		code = curl_easy_perform ((CURL*)val_data(easy_handle));
		
		System::GCExitBlocking ();
		
		lime_curl_easy_flush (easy_handle);
		
		return code;
		
	}
	
	
	HL_PRIM int hl_lime_curl_easy_perform (HL_CFFIPointer* easy_handle) {
		
		int code;
		System::GCEnterBlocking ();
		
		code = curl_easy_perform ((CURL*)easy_handle->ptr);
		
		System::GCExitBlocking ();
		
		hl_lime_curl_easy_flush (easy_handle);
		
		return code;
		
	}
	
	
	int lime_curl_easy_recv (value curl, value buffer, int buflen, int n) {
		
		// TODO
		
		return 0;
		
	}
	
	
	HL_PRIM int hl_lime_curl_easy_recv (HL_CFFIPointer* curl, double buffer, int buflen, int n) {
		
		// TODO
		
		return 0;
		
	}
	
	
	void lime_curl_easy_reset (value curl) {
		
		curl_easy_reset ((CURL*)val_data(curl));
		
	}
	
	
	HL_PRIM void hl_lime_curl_easy_reset (HL_CFFIPointer* curl) {
		
		curl_easy_reset ((CURL*)curl->ptr);
		
	}
	
	
	int lime_curl_easy_send (value curl, value buffer, int buflen, int n) {
		
		// TODO
		
		return 0;
		
	}
	
	
	HL_PRIM int hl_lime_curl_easy_send (HL_CFFIPointer* curl, double buffer, int buflen, int n) {
		
		// TODO
		
		return 0;
		
	}
	
	
	static size_t header_callback (void *ptr, size_t size, size_t nmemb, void *userp) {
		
		std::vector<char*>* values = headerValues[userp];
		
		if (size * nmemb > 0) {
			
			char* data = (char*)malloc (size * nmemb + 1);
			memcpy (data, ptr, size * nmemb);
			data[size * nmemb] = '\0';
			values->push_back (data);
			
		}
		
		return size * nmemb;
		
	}
	
	
	static size_t write_callback (void *ptr, size_t size, size_t nmemb, void *userp) {
		
		std::stringbuf* buffer = writeBuffers[userp];
		
		if (size * nmemb < 1) {
			
			return 0;
			
		}
		
		buffer->sputn ((char*)ptr, size * nmemb);
		return size * nmemb;
		
	}
	
	
	static size_t read_callback (void *buffer, size_t size, size_t nmemb, void *userp) {
		
		Bytes* bytes = readBytes[userp];
		int position = readBytesPosition[userp];
		int length = size * nmemb;
		
		if (bytes->length < position + length) {
			
			length = bytes->length - position;
			
		}
		
		if (length <= 0) return 0;
		
		memcpy (buffer, bytes->b + position, length);
		readBytesPosition[userp] = position + length;
		
		return length;
		
	}
	
	
	static int progress_callback (void *userp, double dltotal, double dlnow, double ultotal, double ulnow) {
		
		CURL_Progress* progress = progressValues[userp];
		
		progress->dltotal = dltotal;
		progress->dlnow = dlnow;
		progress->ultotal = ultotal;
		progress->ulnow = ulnow;
		
		return 0;
		
	}
	
	
	static int xferinfo_callback (void *userp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
		
		CURL_XferInfo* xferInfo = xferInfoValues[userp];
		
		xferInfo->dltotal = dltotal;
		xferInfo->dlnow = dlnow;
		xferInfo->ultotal = ultotal;
		xferInfo->ulnow = ulnow;
		
		return 0;
		
	}
	
	
	int lime_curl_easy_setopt (value handle, int option, value parameter, value bytes) {
		
		CURLcode code = CURLE_OK;
		CURL* easy_handle = (CURL*)val_data(handle);
		CURLoption type = (CURLoption)option;
		
		switch (type) {
			
			case CURLOPT_VERBOSE:
			case CURLOPT_HEADER:
			case CURLOPT_NOPROGRESS:
			case CURLOPT_NOSIGNAL:
			case CURLOPT_WILDCARDMATCH:
			case CURLOPT_FAILONERROR:
			case CURLOPT_DNS_USE_GLOBAL_CACHE:
			case CURLOPT_TCP_NODELAY:
			case CURLOPT_TCP_KEEPALIVE:
			case CURLOPT_SASL_IR:
			case CURLOPT_AUTOREFERER:
			case CURLOPT_TRANSFER_ENCODING:
			case CURLOPT_FOLLOWLOCATION:
			case CURLOPT_UNRESTRICTED_AUTH:
			case CURLOPT_PUT:
			case CURLOPT_POST:
			case CURLOPT_COOKIESESSION:
			case CURLOPT_HTTPGET:
			case CURLOPT_IGNORE_CONTENT_LENGTH:
			case CURLOPT_HTTP_CONTENT_DECODING:
			case CURLOPT_HTTP_TRANSFER_DECODING:
			case CURLOPT_DIRLISTONLY:
			case CURLOPT_APPEND:
			case CURLOPT_FTP_USE_EPRT:
			case CURLOPT_FTP_USE_EPSV:
			case CURLOPT_FTP_USE_PRET:
			case CURLOPT_FTP_CREATE_MISSING_DIRS:
			case CURLOPT_FTP_SKIP_PASV_IP:
			case CURLOPT_TRANSFERTEXT:
			case CURLOPT_CRLF:
			case CURLOPT_NOBODY:
			case CURLOPT_UPLOAD:
			case CURLOPT_FRESH_CONNECT:
			case CURLOPT_FORBID_REUSE:
			case CURLOPT_CONNECT_ONLY:
			case CURLOPT_USE_SSL:
			//case CURLOPT_SSL_ENABLE_ALPN:
			//case CURLOPT_SSL_ENABLE_NPN:
			case CURLOPT_SSL_VERIFYPEER:
			case CURLOPT_SSL_SESSIONID_CACHE:
			case CURLOPT_TCP_FASTOPEN:
			case CURLOPT_KEEP_SENDING_ON_ERROR:
			case CURLOPT_PATH_AS_IS:
			case CURLOPT_SSL_VERIFYSTATUS:
			case CURLOPT_SSL_FALSESTART:
			case CURLOPT_PIPEWAIT:
			case CURLOPT_TFTP_NO_OPTIONS:
			case CURLOPT_SUPPRESS_CONNECT_HEADERS:
			case CURLOPT_SSH_COMPRESSION:
				
				code = curl_easy_setopt (easy_handle, type, val_bool (parameter));
				break;
			
			case CURLOPT_SSL_VERIFYHOST:
			case CURLOPT_PROTOCOLS:
			case CURLOPT_REDIR_PROTOCOLS:
			case CURLOPT_PROXYPORT:
			case CURLOPT_PROXYTYPE:
			case CURLOPT_HTTPPROXYTUNNEL:
			case CURLOPT_SOCKS5_GSSAPI_NEC:
			case CURLOPT_LOCALPORT:
			case CURLOPT_LOCALPORTRANGE:
			case CURLOPT_DNS_CACHE_TIMEOUT:
			case CURLOPT_BUFFERSIZE:
			case CURLOPT_PORT:
			case CURLOPT_ADDRESS_SCOPE:
			case CURLOPT_TCP_KEEPIDLE:
			case CURLOPT_TCP_KEEPINTVL:
			case CURLOPT_NETRC:
			case CURLOPT_HTTPAUTH:
			case CURLOPT_PROXYAUTH:
			case CURLOPT_MAXREDIRS:
			case CURLOPT_POSTREDIR:
			case CURLOPT_POSTFIELDSIZE:
			//case CURLOPT_HEADEROPT:
			case CURLOPT_HTTP_VERSION:
			//case CURLOPT_EXPECT_100_TIMEOUT_MS:
			case CURLOPT_TFTP_BLKSIZE:
			case CURLOPT_FTP_RESPONSE_TIMEOUT:
			case CURLOPT_FTPSSLAUTH:
			case CURLOPT_FTP_SSL_CCC:
			case CURLOPT_FTP_FILEMETHOD:
			case CURLOPT_RTSP_REQUEST:
			case CURLOPT_RTSP_CLIENT_CSEQ:
			case CURLOPT_RTSP_SERVER_CSEQ:
			case CURLOPT_PROXY_TRANSFER_MODE:
			case CURLOPT_RESUME_FROM:
			case CURLOPT_FILETIME:
			case CURLOPT_INFILESIZE:
			case CURLOPT_MAXFILESIZE:
			case CURLOPT_TIMECONDITION:
			case CURLOPT_TIMEVALUE:
			case CURLOPT_TIMEOUT:
			case CURLOPT_TIMEOUT_MS:
			case CURLOPT_LOW_SPEED_LIMIT:
			case CURLOPT_LOW_SPEED_TIME:
			case CURLOPT_MAXCONNECTS:
			case CURLOPT_CONNECTTIMEOUT:
			case CURLOPT_CONNECTTIMEOUT_MS:
			case CURLOPT_IPRESOLVE:
			case CURLOPT_ACCEPTTIMEOUT_MS:
			case CURLOPT_SSLENGINE_DEFAULT:
			case CURLOPT_SSLVERSION:
			case CURLOPT_CERTINFO:
			case CURLOPT_SSL_OPTIONS:
			case CURLOPT_GSSAPI_DELEGATION:
			case CURLOPT_SSH_AUTH_TYPES:
			case CURLOPT_NEW_FILE_PERMS:
			case CURLOPT_NEW_DIRECTORY_PERMS:
			case CURLOPT_STREAM_WEIGHT:
			case CURLOPT_PROXY_SSL_VERIFYPEER:
			case CURLOPT_PROXY_SSL_VERIFYHOST:
			case CURLOPT_PROXY_SSLVERSION:
			case CURLOPT_PROXY_SSL_OPTIONS:
			case CURLOPT_SOCKS5_AUTH:
				
				code = curl_easy_setopt (easy_handle, type, val_int (parameter));
				break;
			
			case CURLOPT_POSTFIELDSIZE_LARGE:
			case CURLOPT_RESUME_FROM_LARGE:
			case CURLOPT_INFILESIZE_LARGE:
			case CURLOPT_MAXFILESIZE_LARGE:
			case CURLOPT_MAX_SEND_SPEED_LARGE:
			case CURLOPT_MAX_RECV_SPEED_LARGE:
				
				code = curl_easy_setopt (easy_handle, type, val_float (parameter));
				break;
			
			case CURLOPT_ERRORBUFFER:
			case CURLOPT_URL:
			case CURLOPT_PROXY:
			case CURLOPT_NOPROXY:
			case CURLOPT_SOCKS5_GSSAPI_SERVICE:
			case CURLOPT_INTERFACE:
			case CURLOPT_NETRC_FILE:
			case CURLOPT_USERPWD:
			case CURLOPT_PROXYUSERPWD:
			case CURLOPT_USERNAME:
			case CURLOPT_PASSWORD:
			case CURLOPT_LOGIN_OPTIONS:
			case CURLOPT_PROXYUSERNAME:
			case CURLOPT_PROXYPASSWORD:
			case CURLOPT_TLSAUTH_USERNAME:
			case CURLOPT_TLSAUTH_PASSWORD:
			case CURLOPT_XOAUTH2_BEARER:
			case CURLOPT_ACCEPT_ENCODING:
			case CURLOPT_POSTFIELDS:
			case CURLOPT_COPYPOSTFIELDS:
			case CURLOPT_REFERER:
			case CURLOPT_USERAGENT:
			case CURLOPT_COOKIE:
			case CURLOPT_COOKIEFILE:
			case CURLOPT_COOKIEJAR:
			case CURLOPT_COOKIELIST:
			case CURLOPT_MAIL_FROM:
			case CURLOPT_MAIL_AUTH:
			case CURLOPT_FTPPORT:
			case CURLOPT_FTP_ALTERNATIVE_TO_USER:
			case CURLOPT_FTP_ACCOUNT:
			case CURLOPT_RTSP_SESSION_ID:
			case CURLOPT_RTSP_STREAM_URI:
			case CURLOPT_RTSP_TRANSPORT:
			case CURLOPT_RANGE:
			case CURLOPT_CUSTOMREQUEST:
			case CURLOPT_DNS_INTERFACE:
			case CURLOPT_DNS_LOCAL_IP4:
			case CURLOPT_DNS_LOCAL_IP6:
			case CURLOPT_SSLCERT:
			case CURLOPT_SSLCERTTYPE:
			case CURLOPT_SSLKEY:
			case CURLOPT_SSLKEYTYPE:
			case CURLOPT_KEYPASSWD:
			case CURLOPT_SSLENGINE:
			case CURLOPT_CAINFO:
			case CURLOPT_ISSUERCERT:
			case CURLOPT_CAPATH:
			case CURLOPT_CRLFILE:
			case CURLOPT_RANDOM_FILE:
			case CURLOPT_EGDSOCKET:
			case CURLOPT_SSL_CIPHER_LIST:
			case CURLOPT_KRBLEVEL:
			case CURLOPT_SSH_HOST_PUBLIC_KEY_MD5:
			case CURLOPT_SSH_PUBLIC_KEYFILE:
			case CURLOPT_SSH_PRIVATE_KEYFILE:
			case CURLOPT_SSH_KNOWNHOSTS:
			case CURLOPT_PINNEDPUBLICKEY:
			case CURLOPT_UNIX_SOCKET_PATH:
			case CURLOPT_PROXY_SERVICE_NAME:
			case CURLOPT_SERVICE_NAME:
			case CURLOPT_DEFAULT_PROTOCOL:
			case CURLOPT_PROXY_CAINFO:
			case CURLOPT_PROXY_CAPATH:
			case CURLOPT_PROXY_TLSAUTH_USERNAME:
			case CURLOPT_PROXY_TLSAUTH_PASSWORD:
			case CURLOPT_PROXY_TLSAUTH_TYPE:
			case CURLOPT_PROXY_SSLCERT:
			case CURLOPT_PROXY_SSLCERTTYPE:
			case CURLOPT_PROXY_SSLKEY:
			case CURLOPT_PROXY_SSLKEYTYPE:
			case CURLOPT_PROXY_KEYPASSWD:
			case CURLOPT_PROXY_SSL_CIPHER_LIST:
			case CURLOPT_PROXY_CRLFILE:
			case CURLOPT_PRE_PROXY:
			case CURLOPT_PROXY_PINNEDPUBLICKEY:
			case CURLOPT_ABSTRACT_UNIX_SOCKET:
			case CURLOPT_REQUEST_TARGET:
				
				code = curl_easy_setopt (easy_handle, type, val_string (parameter));
				break;
			
			case CURLOPT_IOCTLFUNCTION:
			case CURLOPT_IOCTLDATA:
			case CURLOPT_SEEKFUNCTION:
			case CURLOPT_SEEKDATA:
			case CURLOPT_SOCKOPTFUNCTION:
			case CURLOPT_SOCKOPTDATA:
			case CURLOPT_OPENSOCKETFUNCTION:
			case CURLOPT_OPENSOCKETDATA:
			case CURLOPT_CLOSESOCKETFUNCTION:
			case CURLOPT_CLOSESOCKETDATA:
			case CURLOPT_DEBUGFUNCTION:
			case CURLOPT_DEBUGDATA:
			case CURLOPT_SSL_CTX_FUNCTION:
			case CURLOPT_SSL_CTX_DATA:
			case CURLOPT_CONV_TO_NETWORK_FUNCTION:
			case CURLOPT_CONV_FROM_NETWORK_FUNCTION:
			case CURLOPT_CONV_FROM_UTF8_FUNCTION:
			case CURLOPT_INTERLEAVEFUNCTION:
			case CURLOPT_INTERLEAVEDATA:
			case CURLOPT_CHUNK_BGN_FUNCTION:
			case CURLOPT_CHUNK_END_FUNCTION:
			case CURLOPT_CHUNK_DATA:
			case CURLOPT_FNMATCH_FUNCTION:
			case CURLOPT_FNMATCH_DATA:
			case CURLOPT_STDERR:
			case CURLOPT_HTTPPOST:
			//case CURLOPT_PROXYHEADER:
			case CURLOPT_HTTP200ALIASES:
			case CURLOPT_MAIL_RCPT:
			case CURLOPT_QUOTE:
			case CURLOPT_POSTQUOTE:
			case CURLOPT_PREQUOTE:
			case CURLOPT_RESOLVE:
			case CURLOPT_SSH_KEYFUNCTION:
			case CURLOPT_SSH_KEYDATA:
			case CURLOPT_PRIVATE:
			case CURLOPT_SHARE:
			case CURLOPT_TELNETOPTIONS:
			case CURLOPT_STREAM_DEPENDS:
			case CURLOPT_STREAM_DEPENDS_E:
			case CURLOPT_CONNECT_TO:
			case CURLOPT_MIMEPOST:
				
				//todo
				break;
			
			//case CURLOPT_READDATA:
			//case CURLOPT_WRITEDATA:
			//case CURLOPT_HEADERDATA:
			//case CURLOPT_PROGRESSDATA:
			
			case CURLOPT_READFUNCTION:
			{
				// curl_gc_mutex.Lock ();
				// ValuePointer* callback = new ValuePointer (parameter);
				// readCallbacks[handle] = callback;
				// code = curl_easy_setopt (easy_handle, type, read_callback);
				// curl_easy_setopt (easy_handle, CURLOPT_READDATA, handle);
				// curl_gc_mutex.Unlock ();
				break;
			}
			case CURLOPT_READDATA:
			{
				curl_gc_mutex.Lock ();
				
				if (readBytes.find (handle) == readBytes.end ()) {
					
					delete readBytes[handle];
					delete readBytesRoot[handle];
					
				}
				
				Bytes* _readBytes = new Bytes (bytes);
				readBytes[handle] = _readBytes;
				readBytesPosition[handle] = 0;
				readBytesRoot[handle] = new ValuePointer (bytes);
				
				code = curl_easy_setopt (easy_handle, CURLOPT_READFUNCTION, read_callback);
				curl_easy_setopt (easy_handle, CURLOPT_READDATA, handle);
				
				curl_gc_mutex.Unlock ();
				break;
			}
			case CURLOPT_WRITEFUNCTION:
			{
				curl_gc_mutex.Lock ();
				
				if (writeCallbacks.find (handle) == writeCallbacks.end ()) {
					
					delete writeCallbacks[handle];
					delete writeBytes[handle];
					delete writeBytesRoot[handle];
					
				}
				
				ValuePointer* callback = new ValuePointer (parameter);
				writeCallbacks[handle] = callback;
				Bytes* _writeBytes = new Bytes (bytes);
				writeBytes[handle] = _writeBytes;
				writeBytesRoot[handle] = new ValuePointer (bytes);
				
				code = curl_easy_setopt (easy_handle, type, write_callback);
				curl_easy_setopt (easy_handle, CURLOPT_WRITEDATA, handle);
				
				curl_gc_mutex.Unlock ();
				break;
			}
			case CURLOPT_HEADERFUNCTION:
			{
				curl_gc_mutex.Lock ();
				
				if (headerCallbacks.find (handle) == headerCallbacks.end ()) {
					
					delete headerCallbacks[handle];
					delete headerValues[handle];
					
				}
				
				ValuePointer* callback = new ValuePointer (parameter);
				headerCallbacks[handle] = callback;
				headerValues[handle] = new std::vector<char*> ();
				
				code = curl_easy_setopt (easy_handle, type, header_callback);
				curl_easy_setopt (easy_handle, CURLOPT_HEADERDATA, handle);
				
				curl_gc_mutex.Unlock ();
				break;
			}
			case CURLOPT_PROGRESSFUNCTION:
			{
				curl_gc_mutex.Lock ();
				
				if (progressCallbacks.find (handle) != progressCallbacks.end ()) {
					
					delete progressCallbacks[handle];
					delete progressValues[handle];
					
				}
				
				progressCallbacks[handle] = new ValuePointer (parameter);;
				progressValues[handle] = new CURL_Progress ();
				
				code = curl_easy_setopt (easy_handle, type, progress_callback);
				curl_easy_setopt (easy_handle, CURLOPT_PROGRESSDATA, handle);
				curl_easy_setopt (easy_handle, CURLOPT_NOPROGRESS, false);
				
				curl_gc_mutex.Unlock ();
				break;
			}
			case CURLOPT_XFERINFOFUNCTION:
			{
				curl_gc_mutex.Lock ();
				
				if (xferInfoCallbacks.find (handle) != xferInfoCallbacks.end ()) {
					
					delete xferInfoCallbacks[handle];
					delete xferInfoValues[handle];
					
				}
				
				xferInfoCallbacks[handle] = new ValuePointer (parameter);
				xferInfoValues[handle] = new CURL_XferInfo ();
				
				code = curl_easy_setopt (easy_handle, type, xferinfo_callback);
				curl_easy_setopt (easy_handle, CURLOPT_XFERINFODATA, handle);
				curl_easy_setopt (easy_handle, CURLOPT_NOPROGRESS, false);
				
				curl_gc_mutex.Unlock ();
				break;
			}
			
			case CURLOPT_HTTPHEADER:
			{
				curl_gc_mutex.Lock ();
				
				if (headerSLists.find (handle) != headerSLists.end ()) {
					
					curl_slist_free_all (headerSLists[handle]);
					
				}
				
				struct curl_slist *chunk = NULL;
				int size = val_array_size (parameter);
				
				for (int i = 0; i < size; i++) {
					
					chunk = curl_slist_append (chunk, val_string (val_array_i (parameter, i)));
					
				}
				
				headerSLists[handle] = chunk;
				
				code = curl_easy_setopt (easy_handle, type, chunk);
				curl_gc_mutex.Unlock ();
				break;
			}
			
			default:
				
				break;
			
		}
		
		return code;
		
	}
	
	
	HL_PRIM int hl_lime_curl_easy_setopt (HL_CFFIPointer* handle, int option, vdynamic* parameter, Bytes* bytes) {
		
		CURLcode code = CURLE_OK;
		CURL* easy_handle = (CURL*)handle->ptr;
		CURLoption type = (CURLoption)option;
		
		switch (type) {
			
			case CURLOPT_VERBOSE:
			case CURLOPT_HEADER:
			case CURLOPT_NOPROGRESS:
			case CURLOPT_NOSIGNAL:
			case CURLOPT_WILDCARDMATCH:
			case CURLOPT_FAILONERROR:
			case CURLOPT_DNS_USE_GLOBAL_CACHE:
			case CURLOPT_TCP_NODELAY:
			case CURLOPT_TCP_KEEPALIVE:
			case CURLOPT_SASL_IR:
			case CURLOPT_AUTOREFERER:
			case CURLOPT_TRANSFER_ENCODING:
			case CURLOPT_FOLLOWLOCATION:
			case CURLOPT_UNRESTRICTED_AUTH:
			case CURLOPT_PUT:
			case CURLOPT_POST:
			case CURLOPT_COOKIESESSION:
			case CURLOPT_HTTPGET:
			case CURLOPT_IGNORE_CONTENT_LENGTH:
			case CURLOPT_HTTP_CONTENT_DECODING:
			case CURLOPT_HTTP_TRANSFER_DECODING:
			case CURLOPT_DIRLISTONLY:
			case CURLOPT_APPEND:
			case CURLOPT_FTP_USE_EPRT:
			case CURLOPT_FTP_USE_EPSV:
			case CURLOPT_FTP_USE_PRET:
			case CURLOPT_FTP_CREATE_MISSING_DIRS:
			case CURLOPT_FTP_SKIP_PASV_IP:
			case CURLOPT_TRANSFERTEXT:
			case CURLOPT_CRLF:
			case CURLOPT_NOBODY:
			case CURLOPT_UPLOAD:
			case CURLOPT_FRESH_CONNECT:
			case CURLOPT_FORBID_REUSE:
			case CURLOPT_CONNECT_ONLY:
			case CURLOPT_USE_SSL:
			//case CURLOPT_SSL_ENABLE_ALPN:
			//case CURLOPT_SSL_ENABLE_NPN:
			case CURLOPT_SSL_VERIFYPEER:
			case CURLOPT_SSL_SESSIONID_CACHE:
			case CURLOPT_TCP_FASTOPEN:
			case CURLOPT_KEEP_SENDING_ON_ERROR:
			case CURLOPT_PATH_AS_IS:
			case CURLOPT_SSL_VERIFYSTATUS:
			case CURLOPT_SSL_FALSESTART:
			case CURLOPT_PIPEWAIT:
			case CURLOPT_TFTP_NO_OPTIONS:
			case CURLOPT_SUPPRESS_CONNECT_HEADERS:
			case CURLOPT_SSH_COMPRESSION:
				
				code = curl_easy_setopt (easy_handle, type, parameter->v.b);
				break;
			
			case CURLOPT_SSL_VERIFYHOST:
			case CURLOPT_PROTOCOLS:
			case CURLOPT_REDIR_PROTOCOLS:
			case CURLOPT_PROXYPORT:
			case CURLOPT_PROXYTYPE:
			case CURLOPT_HTTPPROXYTUNNEL:
			case CURLOPT_SOCKS5_GSSAPI_NEC:
			case CURLOPT_LOCALPORT:
			case CURLOPT_LOCALPORTRANGE:
			case CURLOPT_DNS_CACHE_TIMEOUT:
			case CURLOPT_BUFFERSIZE:
			case CURLOPT_PORT:
			case CURLOPT_ADDRESS_SCOPE:
			case CURLOPT_TCP_KEEPIDLE:
			case CURLOPT_TCP_KEEPINTVL:
			case CURLOPT_NETRC:
			case CURLOPT_HTTPAUTH:
			case CURLOPT_PROXYAUTH:
			case CURLOPT_MAXREDIRS:
			case CURLOPT_POSTREDIR:
			case CURLOPT_POSTFIELDSIZE:
			//case CURLOPT_HEADEROPT:
			case CURLOPT_HTTP_VERSION:
			//case CURLOPT_EXPECT_100_TIMEOUT_MS:
			case CURLOPT_TFTP_BLKSIZE:
			case CURLOPT_FTP_RESPONSE_TIMEOUT:
			case CURLOPT_FTPSSLAUTH:
			case CURLOPT_FTP_SSL_CCC:
			case CURLOPT_FTP_FILEMETHOD:
			case CURLOPT_RTSP_REQUEST:
			case CURLOPT_RTSP_CLIENT_CSEQ:
			case CURLOPT_RTSP_SERVER_CSEQ:
			case CURLOPT_PROXY_TRANSFER_MODE:
			case CURLOPT_RESUME_FROM:
			case CURLOPT_FILETIME:
			case CURLOPT_INFILESIZE:
			case CURLOPT_MAXFILESIZE:
			case CURLOPT_TIMECONDITION:
			case CURLOPT_TIMEVALUE:
			case CURLOPT_TIMEOUT:
			case CURLOPT_TIMEOUT_MS:
			case CURLOPT_LOW_SPEED_LIMIT:
			case CURLOPT_LOW_SPEED_TIME:
			case CURLOPT_MAXCONNECTS:
			case CURLOPT_CONNECTTIMEOUT:
			case CURLOPT_CONNECTTIMEOUT_MS:
			case CURLOPT_IPRESOLVE:
			case CURLOPT_ACCEPTTIMEOUT_MS:
			case CURLOPT_SSLENGINE_DEFAULT:
			case CURLOPT_SSLVERSION:
			case CURLOPT_CERTINFO:
			case CURLOPT_SSL_OPTIONS:
			case CURLOPT_GSSAPI_DELEGATION:
			case CURLOPT_SSH_AUTH_TYPES:
			case CURLOPT_NEW_FILE_PERMS:
			case CURLOPT_NEW_DIRECTORY_PERMS:
			case CURLOPT_STREAM_WEIGHT:
			case CURLOPT_PROXY_SSL_VERIFYPEER:
			case CURLOPT_PROXY_SSL_VERIFYHOST:
			case CURLOPT_PROXY_SSLVERSION:
			case CURLOPT_PROXY_SSL_OPTIONS:
			case CURLOPT_SOCKS5_AUTH:
				
				code = curl_easy_setopt (easy_handle, type, parameter->v.i);
				break;
			
			case CURLOPT_POSTFIELDSIZE_LARGE:
			case CURLOPT_RESUME_FROM_LARGE:
			case CURLOPT_INFILESIZE_LARGE:
			case CURLOPT_MAXFILESIZE_LARGE:
			case CURLOPT_MAX_SEND_SPEED_LARGE:
			case CURLOPT_MAX_RECV_SPEED_LARGE:
				
				code = curl_easy_setopt (easy_handle, type, parameter->v.f);
				break;
			
			case CURLOPT_ERRORBUFFER:
			case CURLOPT_URL:
			case CURLOPT_PROXY:
			case CURLOPT_NOPROXY:
			case CURLOPT_SOCKS5_GSSAPI_SERVICE:
			case CURLOPT_INTERFACE:
			case CURLOPT_NETRC_FILE:
			case CURLOPT_USERPWD:
			case CURLOPT_PROXYUSERPWD:
			case CURLOPT_USERNAME:
			case CURLOPT_PASSWORD:
			case CURLOPT_LOGIN_OPTIONS:
			case CURLOPT_PROXYUSERNAME:
			case CURLOPT_PROXYPASSWORD:
			case CURLOPT_TLSAUTH_USERNAME:
			case CURLOPT_TLSAUTH_PASSWORD:
			case CURLOPT_XOAUTH2_BEARER:
			case CURLOPT_ACCEPT_ENCODING:
			case CURLOPT_POSTFIELDS:
			case CURLOPT_COPYPOSTFIELDS:
			case CURLOPT_REFERER:
			case CURLOPT_USERAGENT:
			case CURLOPT_COOKIE:
			case CURLOPT_COOKIEFILE:
			case CURLOPT_COOKIEJAR:
			case CURLOPT_COOKIELIST:
			case CURLOPT_MAIL_FROM:
			case CURLOPT_MAIL_AUTH:
			case CURLOPT_FTPPORT:
			case CURLOPT_FTP_ALTERNATIVE_TO_USER:
			case CURLOPT_FTP_ACCOUNT:
			case CURLOPT_RTSP_SESSION_ID:
			case CURLOPT_RTSP_STREAM_URI:
			case CURLOPT_RTSP_TRANSPORT:
			case CURLOPT_RANGE:
			case CURLOPT_CUSTOMREQUEST:
			case CURLOPT_DNS_INTERFACE:
			case CURLOPT_DNS_LOCAL_IP4:
			case CURLOPT_DNS_LOCAL_IP6:
			case CURLOPT_SSLCERT:
			case CURLOPT_SSLCERTTYPE:
			case CURLOPT_SSLKEY:
			case CURLOPT_SSLKEYTYPE:
			case CURLOPT_KEYPASSWD:
			case CURLOPT_SSLENGINE:
			case CURLOPT_CAINFO:
			case CURLOPT_ISSUERCERT:
			case CURLOPT_CAPATH:
			case CURLOPT_CRLFILE:
			case CURLOPT_RANDOM_FILE:
			case CURLOPT_EGDSOCKET:
			case CURLOPT_SSL_CIPHER_LIST:
			case CURLOPT_KRBLEVEL:
			case CURLOPT_SSH_HOST_PUBLIC_KEY_MD5:
			case CURLOPT_SSH_PUBLIC_KEYFILE:
			case CURLOPT_SSH_PRIVATE_KEYFILE:
			case CURLOPT_SSH_KNOWNHOSTS:
			case CURLOPT_PINNEDPUBLICKEY:
			case CURLOPT_UNIX_SOCKET_PATH:
			case CURLOPT_PROXY_SERVICE_NAME:
			case CURLOPT_SERVICE_NAME:
			case CURLOPT_DEFAULT_PROTOCOL:
			case CURLOPT_PROXY_CAINFO:
			case CURLOPT_PROXY_CAPATH:
			case CURLOPT_PROXY_TLSAUTH_USERNAME:
			case CURLOPT_PROXY_TLSAUTH_PASSWORD:
			case CURLOPT_PROXY_TLSAUTH_TYPE:
			case CURLOPT_PROXY_SSLCERT:
			case CURLOPT_PROXY_SSLCERTTYPE:
			case CURLOPT_PROXY_SSLKEY:
			case CURLOPT_PROXY_SSLKEYTYPE:
			case CURLOPT_PROXY_KEYPASSWD:
			case CURLOPT_PROXY_SSL_CIPHER_LIST:
			case CURLOPT_PROXY_CRLFILE:
			case CURLOPT_PRE_PROXY:
			case CURLOPT_PROXY_PINNEDPUBLICKEY:
			case CURLOPT_ABSTRACT_UNIX_SOCKET:
			case CURLOPT_REQUEST_TARGET:
			{
				hl_vstring* str = (hl_vstring*)parameter;
				code = curl_easy_setopt (easy_handle, type, str ? hl_to_utf8 (str->bytes) : NULL);
				break;
			}
			
			case CURLOPT_IOCTLFUNCTION:
			case CURLOPT_IOCTLDATA:
			case CURLOPT_SEEKFUNCTION:
			case CURLOPT_SEEKDATA:
			case CURLOPT_SOCKOPTFUNCTION:
			case CURLOPT_SOCKOPTDATA:
			case CURLOPT_OPENSOCKETFUNCTION:
			case CURLOPT_OPENSOCKETDATA:
			case CURLOPT_CLOSESOCKETFUNCTION:
			case CURLOPT_CLOSESOCKETDATA:
			case CURLOPT_DEBUGFUNCTION:
			case CURLOPT_DEBUGDATA:
			case CURLOPT_SSL_CTX_FUNCTION:
			case CURLOPT_SSL_CTX_DATA:
			case CURLOPT_CONV_TO_NETWORK_FUNCTION:
			case CURLOPT_CONV_FROM_NETWORK_FUNCTION:
			case CURLOPT_CONV_FROM_UTF8_FUNCTION:
			case CURLOPT_INTERLEAVEFUNCTION:
			case CURLOPT_INTERLEAVEDATA:
			case CURLOPT_CHUNK_BGN_FUNCTION:
			case CURLOPT_CHUNK_END_FUNCTION:
			case CURLOPT_CHUNK_DATA:
			case CURLOPT_FNMATCH_FUNCTION:
			case CURLOPT_FNMATCH_DATA:
			case CURLOPT_STDERR:
			case CURLOPT_HTTPPOST:
			//case CURLOPT_PROXYHEADER:
			case CURLOPT_HTTP200ALIASES:
			case CURLOPT_MAIL_RCPT:
			case CURLOPT_QUOTE:
			case CURLOPT_POSTQUOTE:
			case CURLOPT_PREQUOTE:
			case CURLOPT_RESOLVE:
			case CURLOPT_SSH_KEYFUNCTION:
			case CURLOPT_SSH_KEYDATA:
			case CURLOPT_PRIVATE:
			case CURLOPT_SHARE:
			case CURLOPT_TELNETOPTIONS:
			case CURLOPT_STREAM_DEPENDS:
			case CURLOPT_STREAM_DEPENDS_E:
			case CURLOPT_CONNECT_TO:
			case CURLOPT_MIMEPOST:
				
				//todo
				break;
			
			//case CURLOPT_READDATA:
			//case CURLOPT_WRITEDATA:
			//case CURLOPT_HEADERDATA:
			//case CURLOPT_PROGRESSDATA:
			
			case CURLOPT_READFUNCTION:
			{
				// curl_gc_mutex.Lock ();
				// ValuePointer* callback = new ValuePointer (parameter);
				// readCallbacks[handle] = callback;
				// code = curl_easy_setopt (easy_handle, type, read_callback);
				// curl_easy_setopt (easy_handle, CURLOPT_READDATA, handle);
				// curl_gc_mutex.Unlock ();
				break;
			}
			case CURLOPT_READDATA:
			{
				curl_gc_mutex.Lock ();
				
				if (readBytes.find (handle) == readBytes.end ()) {
					
					delete readBytesRoot[handle];
					
				}
				
				readBytes[handle] = bytes;
				readBytesPosition[handle] = 0;
				readBytesRoot[handle] = new ValuePointer ((vobj*)bytes);
				
				code = curl_easy_setopt (easy_handle, CURLOPT_READFUNCTION, read_callback);
				curl_easy_setopt (easy_handle, CURLOPT_READDATA, handle);
				
				curl_gc_mutex.Unlock ();
				break;
			}
			case CURLOPT_WRITEFUNCTION:
			{
				curl_gc_mutex.Lock ();
				
				if (writeCallbacks.find (handle) == writeCallbacks.end ()) {
					
					delete writeCallbacks[handle];
					delete writeBytesRoot[handle];
					
				}
				
				ValuePointer* callback = new ValuePointer (parameter);
				writeCallbacks[handle] = callback;
				writeBytes[handle] = bytes;
				writeBytesRoot[handle] = new ValuePointer ((vobj*)bytes);
				
				code = curl_easy_setopt (easy_handle, type, write_callback);
				curl_easy_setopt (easy_handle, CURLOPT_WRITEDATA, handle);
				
				curl_gc_mutex.Unlock ();
				break;
			}
			case CURLOPT_HEADERFUNCTION:
			{
				curl_gc_mutex.Lock ();
				
				if (headerCallbacks.find (handle) == headerCallbacks.end ()) {
					
					delete headerCallbacks[handle];
					delete headerValues[handle];
					
				}
				
				ValuePointer* callback = new ValuePointer (parameter);
				headerCallbacks[handle] = callback;
				headerValues[handle] = new std::vector<char*> ();
				
				code = curl_easy_setopt (easy_handle, type, header_callback);
				curl_easy_setopt (easy_handle, CURLOPT_HEADERDATA, handle);
				
				curl_gc_mutex.Unlock ();
				break;
			}
			case CURLOPT_PROGRESSFUNCTION:
			{
				curl_gc_mutex.Lock ();
				
				if (progressCallbacks.find (handle) != progressCallbacks.end ()) {
					
					delete progressCallbacks[handle];
					delete progressValues[handle];
					
				}
				
				progressCallbacks[handle] = new ValuePointer (parameter);
				progressValues[handle] = new CURL_Progress ();
				
				code = curl_easy_setopt (easy_handle, type, progress_callback);
				curl_easy_setopt (easy_handle, CURLOPT_PROGRESSDATA, handle);
				curl_easy_setopt (easy_handle, CURLOPT_NOPROGRESS, false);
				
				curl_gc_mutex.Unlock ();
				break;
			}
			case CURLOPT_XFERINFOFUNCTION:
			{
				curl_gc_mutex.Lock ();
				
				if (xferInfoCallbacks.find (handle) != xferInfoCallbacks.end ()) {
					
					delete xferInfoCallbacks[handle];
					delete xferInfoValues[handle];
					
				}
				
				xferInfoCallbacks[handle] = new ValuePointer (parameter);
				xferInfoValues[handle] = new CURL_XferInfo ();
				
				code = curl_easy_setopt (easy_handle, type, xferinfo_callback);
				curl_easy_setopt (easy_handle, CURLOPT_XFERINFODATA, handle);
				curl_easy_setopt (easy_handle, CURLOPT_NOPROGRESS, false);
				
				curl_gc_mutex.Unlock ();
				break;
			}
			
			case CURLOPT_HTTPHEADER:
			{
				curl_gc_mutex.Lock ();
				
				if (headerSLists.find (handle) != headerSLists.end ()) {
					
					curl_slist_free_all (headerSLists[handle]);
					
				}
				
				struct curl_slist *chunk = NULL;
				varray* stringList = (varray*)parameter;
				hl_vstring** stringListData = hl_aptr (stringList, hl_vstring*);
				int size = stringList->size;
				hl_vstring* data;
				
				for (int i = 0; i < size; i++) {
					
					data = *stringListData++;
					chunk = curl_slist_append (chunk, data ? hl_to_utf8 (data->bytes) : NULL);
					
				}
				
				headerSLists[handle] = chunk;
				
				code = curl_easy_setopt (easy_handle, type, chunk);
				curl_gc_mutex.Unlock ();
				break;
			}
			
			default:
				
				break;
			
		}
		
		return code;
		
	}
	
	
	value lime_curl_easy_strerror (int errornum) {
		
		const char* result = curl_easy_strerror ((CURLcode)errornum);
		return result ? alloc_string (result) : alloc_null ();
		
	}
	
	
	HL_PRIM vbyte* hl_lime_curl_easy_strerror (int errornum) {
		
		const char* result = curl_easy_strerror ((CURLcode)errornum);
		return (vbyte*)result;
		
	}
	
	
	value lime_curl_easy_unescape (value curl, HxString url, int inlength, int outlength) {
		
		char* result = curl_easy_unescape ((CURL*)val_data(curl), url.__s, inlength, &outlength);
		return result ? alloc_string (result) : alloc_null ();
		
	}
	
	
	HL_PRIM vbyte* hl_lime_curl_easy_unescape (HL_CFFIPointer* curl, hl_vstring* url, int inlength, int outlength) {
		
		char* result = curl_easy_unescape ((CURL*)curl->ptr, url ? hl_to_utf8 (url->bytes) : NULL, inlength, &outlength);
		return (vbyte*)result;
		
	}
	
	
	//lime_curl_formadd;
	//lime_curl_formfree;
	//lime_curl_formget;
	
	
	double lime_curl_getdate (HxString datestring, double now) {
		
		time_t time = (time_t)now;
		return curl_getdate (datestring.__s, &time);
		
	}
	
	
	HL_PRIM double hl_lime_curl_getdate (hl_vstring* datestring, double now) {
		
		time_t time = (time_t)now;
		return curl_getdate (datestring ? hl_to_utf8 (datestring->bytes) : NULL, &time);
		
	}
	
	
	void lime_curl_global_cleanup () {
		
		curl_global_cleanup ();
		
	}
	
	
	HL_PRIM void hl_lime_curl_global_cleanup () {
		
		curl_global_cleanup ();
		
	}
	
	
	int lime_curl_global_init (int flags) {
		
		return curl_global_init (flags);
		
	}
	
	
	HL_PRIM int hl_lime_curl_global_init (int flags) {
		
		return curl_global_init (flags);
		
	}
	
	
	int lime_curl_multi_cleanup (value multi_handle) {
		
		curl_gc_mutex.Lock ();
		
		// CURLMcode result = curl_multi_cleanup ((CURLM*)val_data (multi_handle));
		gc_curl_multi (multi_handle);
		
		curl_gc_mutex.Unlock ();
		
		return CURLM_OK;
		
	}
	
	
	HL_PRIM int hl_lime_curl_multi_cleanup (HL_CFFIPointer* multi_handle) {
		
		curl_gc_mutex.Lock ();
		
		// CURLMcode result = curl_multi_cleanup ((CURLM*)val_data (multi_handle));
		hl_gc_curl_multi (multi_handle);
		
		curl_gc_mutex.Unlock ();
		
		return CURLM_OK;
		
	}
	
	
	value lime_curl_multi_init () {
		
		curl_gc_mutex.Lock ();
		
		value handle = CFFIPointer (curl_multi_init (), gc_curl_multi);
		
		if (curlMultiValid.find (handle) != curlMultiValid.end ()) {
			
			printf ("Error: Duplicate cURL Multi handle\n");
			
		}
		
		curlMultiValid[handle] = true;
		curlMultiRunningHandles[handle] = 0;
		
		curl_gc_mutex.Unlock ();
		
		return handle;
		
	}
	
	
	HL_PRIM HL_CFFIPointer* hl_lime_curl_multi_init () {
		
		curl_gc_mutex.Lock ();
		
		HL_CFFIPointer* handle = HLCFFIPointer (curl_multi_init (), (hl_finalizer)hl_gc_curl_multi);
		
		if (curlMultiValid.find (handle) != curlMultiValid.end ()) {
			
			printf ("Error: Duplicate cURL Multi handle\n");
			
		}
		
		curlMultiValid[handle] = true;
		curlMultiRunningHandles[handle] = 0;
		
		curl_gc_mutex.Unlock ();
		
		return handle;
		
	}
	
	
	int lime_curl_multi_add_handle (value multi_handle, value curl_handle) {
		
		curl_gc_mutex.Lock ();
		
		CURLMcode result = curl_multi_add_handle ((CURLM*)val_data (multi_handle), (CURL*)val_data (curl_handle));
		
		if (result == CURLM_OK) {
			
			curlMultiHandles[curl_handle] = multi_handle;
			
		}
		
		curl_gc_mutex.Unlock ();
		
		return result;
		
	}
	
	
	HL_PRIM int hl_lime_curl_multi_add_handle (HL_CFFIPointer* multi_handle, HL_CFFIPointer* curl_handle) {
		
		curl_gc_mutex.Lock ();
		
		CURLMcode result = curl_multi_add_handle ((CURLM*)multi_handle->ptr, (CURL*)curl_handle->ptr);
		
		if (result == CURLM_OK) {
			
			curlMultiHandles[curl_handle] = multi_handle;
			
		}
		
		curl_gc_mutex.Unlock ();
		
		return result;
		
	}
	
	
	int lime_curl_multi_get_running_handles (value multi_handle) {
		
		return curlMultiRunningHandles[multi_handle];
		
	}
	
	
	HL_PRIM int hl_lime_curl_multi_get_running_handles (HL_CFFIPointer* multi_handle) {
		
		return curlMultiRunningHandles[multi_handle];
		
	}
	
	
	value lime_curl_multi_info_read (value multi_handle) {
		
		int msgs_in_queue;
		CURLMsg* msg = curl_multi_info_read ((CURLM*)val_data (multi_handle), &msgs_in_queue);
		
		if (msg) {
			
			//const field val_id ("msg");
			const field easy_handle = val_id ("easy_handle");
			const field _result = val_id ("result");
			
			CURL* curl = msg->easy_handle;
			value result = alloc_empty_object ();
			
			if (curlObjects.find (curl) != curlObjects.end ()) {
				
				alloc_field (result, easy_handle, (value)curlObjects[curl]);
				
			} else {
				
				// TODO?
				alloc_field (result, easy_handle, alloc_null ());
				
			}
			
			alloc_field (result, _result, alloc_int (msg->data.result));
			return result;
			
		} else {
			
			return alloc_null ();
			
		}
		
	}
	
	
	HL_PRIM vdynamic* hl_lime_curl_multi_info_read (HL_CFFIPointer* multi_handle, vdynamic* result) {
		
		int msgs_in_queue;
		CURLMsg* msg = curl_multi_info_read ((CURLM*)multi_handle->ptr, &msgs_in_queue);
		
		if (msg) {
			
			//const field val_id ("msg");
			const int easy_handle = hl_hash_utf8 ("easy_handle");
			const int _result = hl_hash_utf8 ("result");
			
			CURL* curl = msg->easy_handle;
			
			if (curlObjects.find (curl) != curlObjects.end ()) {
				
				hl_dyn_setp (result, easy_handle, &hlt_dyn, (value)curlObjects[curl]);
				
			} else {
				
				// TODO?
				hl_dyn_setp (result, easy_handle, &hlt_dyn, NULL);
				
			}
			
			hl_dyn_seti (result, _result, &hlt_i32, msg->data.result);
			return result;
			
		} else {
			
			return NULL;
			
		}
		
	}
	
	
	int lime_curl_multi_perform (value multi_handle) {
		
		curl_gc_mutex.Lock ();
		
		int runningHandles = 0;
		CURLMcode result = curl_multi_perform ((CURLM*)val_data (multi_handle), &runningHandles);
		
		curlMultiRunningHandles[multi_handle] = runningHandles;
		
		curl_gc_mutex.Unlock ();
		
		return result;
		
	}
	
	
	HL_PRIM int hl_lime_curl_multi_perform (HL_CFFIPointer* multi_handle) {
		
		curl_gc_mutex.Lock ();
		
		int runningHandles = 0;
		CURLMcode result = curl_multi_perform ((CURLM*)multi_handle->ptr, &runningHandles);
		
		curlMultiRunningHandles[multi_handle] = runningHandles;
		
		curl_gc_mutex.Unlock ();
		
		return result;
		
	}
	
	
	int lime_curl_multi_remove_handle (value multi_handle, value curl_handle) {
		
		curl_gc_mutex.Lock ();
		
		CURLMcode result = curl_multi_remove_handle ((CURLM*)val_data (multi_handle), (CURL*)val_data (curl_handle));
		
		if (/*result == CURLM_OK &&*/ curlMultiHandles.find (curl_handle) != curlMultiHandles.end ()) {
			
			curlMultiHandles.erase (curl_handle);
			
		}
		
		curl_gc_mutex.Unlock ();
		
		return result;
		
	}
	
	
	HL_PRIM int hl_lime_curl_multi_remove_handle (HL_CFFIPointer* multi_handle, HL_CFFIPointer* curl_handle) {
		
		curl_gc_mutex.Lock ();
		
		CURLMcode result = curl_multi_remove_handle ((CURLM*)multi_handle->ptr, (CURL*)curl_handle->ptr);
		
		if (/*result == CURLM_OK &&*/ curlMultiHandles.find (curl_handle) != curlMultiHandles.end ()) {
			
			curlMultiHandles.erase (curl_handle);
			
		}
		
		curl_gc_mutex.Unlock ();
		
		return result;
		
	}
	
	
	int lime_curl_multi_wait (value multi_handle, int timeout_ms) {
		
		System::GCEnterBlocking ();
		
		int retcode;
		CURLMcode result = curl_multi_wait ((CURLM*)val_data (multi_handle), 0, 0, timeout_ms, &retcode);
		
		System::GCExitBlocking ();
		return result;
		
	}
	
	
	HL_PRIM int hl_lime_curl_multi_wait (HL_CFFIPointer* multi_handle, int timeout_ms) {
		
		System::GCEnterBlocking ();
		
		int retcode;
		CURLMcode result = curl_multi_wait ((CURLM*)multi_handle->ptr, 0, 0, timeout_ms, &retcode);
		
		System::GCExitBlocking ();
		return result;
		
	}
	
	
	//lime_curl_multi_add_handle
	//lime_curl_multi_assign
	//lime_curl_multi_cleanup
	//lime_curl_multi_fdset
	//lime_curl_multi_info_read
	//lime_curl_multi_init
	//lime_curl_multi_perform
	//lime_curl_multi_remove_handle
	//lime_curl_multi_setopt
	//lime_curl_multi_socket
	//lime_curl_multi_socket_action
	//lime_curl_multi_strerror
	//lime_curl_multi_timeout
	
	//lime_curl_share_cleanup
	//lime_curl_share_init
	//lime_curl_share_setopt
	//lime_curl_share_strerror
	
	//lime_curl_slist_append 
	//lime_curl_slist_free_all
	
	
	value lime_curl_version () {
		
		char* result = curl_version ();
		return result ? alloc_string (result) : alloc_null ();
		
	}
	
	
	HL_PRIM vbyte* hl_lime_curl_version () {
		
		char* result = curl_version ();
		return (vbyte*)result;
		
	}
	
	
	value lime_curl_version_info (int type) {
		
		curl_version_info_data* data = curl_version_info ((CURLversion)type);
		
		// TODO
		
		return alloc_null ();
		
	}
	
	
	HL_PRIM vdynamic* hl_lime_curl_version_info (int type) {
		
		curl_version_info_data* data = curl_version_info ((CURLversion)type);
		
		// TODO
		
		return NULL;
		
	}
	
	
	DEFINE_PRIME1v (lime_curl_easy_cleanup);
	DEFINE_PRIME1 (lime_curl_easy_duphandle);
	DEFINE_PRIME3 (lime_curl_easy_escape);
	DEFINE_PRIME2 (lime_curl_easy_getinfo);
	DEFINE_PRIME0 (lime_curl_easy_init);
	DEFINE_PRIME1v (lime_curl_easy_flush);
	DEFINE_PRIME2 (lime_curl_easy_pause);
	DEFINE_PRIME1 (lime_curl_easy_perform);
	DEFINE_PRIME4 (lime_curl_easy_recv);
	DEFINE_PRIME1v (lime_curl_easy_reset);
	DEFINE_PRIME4 (lime_curl_easy_send);
	DEFINE_PRIME4 (lime_curl_easy_setopt);
	DEFINE_PRIME1 (lime_curl_easy_strerror);
	DEFINE_PRIME4 (lime_curl_easy_unescape);
	DEFINE_PRIME2 (lime_curl_getdate);
	DEFINE_PRIME0v (lime_curl_global_cleanup);
	DEFINE_PRIME1 (lime_curl_global_init);
	DEFINE_PRIME1 (lime_curl_multi_cleanup);
	DEFINE_PRIME0 (lime_curl_multi_init);
	DEFINE_PRIME2 (lime_curl_multi_add_handle);
	DEFINE_PRIME1 (lime_curl_multi_get_running_handles);
	DEFINE_PRIME1 (lime_curl_multi_info_read);
	DEFINE_PRIME1 (lime_curl_multi_perform);
	DEFINE_PRIME2 (lime_curl_multi_remove_handle);
	DEFINE_PRIME2 (lime_curl_multi_wait);
	DEFINE_PRIME0 (lime_curl_version);
	DEFINE_PRIME1 (lime_curl_version_info);
	
	
	#define _TBYTES _OBJ (_I32 _BYTES)
	#define _TCFFIPOINTER _DYN
	
	DEFINE_HL_PRIM (_VOID, lime_curl_easy_cleanup, _TCFFIPOINTER);
	DEFINE_HL_PRIM (_TCFFIPOINTER, lime_curl_easy_duphandle, _TCFFIPOINTER);
	DEFINE_HL_PRIM (_BYTES, lime_curl_easy_escape, _TCFFIPOINTER _STRING _I32);
	DEFINE_HL_PRIM (_DYN, lime_curl_easy_getinfo, _TCFFIPOINTER _I32);
	DEFINE_HL_PRIM (_TCFFIPOINTER, lime_curl_easy_init, _NO_ARG);
	DEFINE_HL_PRIM (_VOID, lime_curl_easy_flush, _TCFFIPOINTER);
	DEFINE_HL_PRIM (_I32, lime_curl_easy_pause, _TCFFIPOINTER _I32);
	DEFINE_HL_PRIM (_I32, lime_curl_easy_perform, _TCFFIPOINTER);
	DEFINE_HL_PRIM (_I32, lime_curl_easy_recv, _TCFFIPOINTER _F64 _I32 _I32);
	DEFINE_HL_PRIM (_VOID, lime_curl_easy_reset, _TCFFIPOINTER);
	DEFINE_HL_PRIM (_I32, lime_curl_easy_send, _TCFFIPOINTER _F64 _I32 _I32);
	DEFINE_HL_PRIM (_I32, lime_curl_easy_setopt, _TCFFIPOINTER _I32 _DYN _TBYTES);
	DEFINE_HL_PRIM (_BYTES, lime_curl_easy_strerror, _I32);
	DEFINE_HL_PRIM (_BYTES, lime_curl_easy_unescape, _TCFFIPOINTER _STRING _I32 _I32);
	DEFINE_HL_PRIM (_F64, lime_curl_getdate, _STRING _F64);
	DEFINE_HL_PRIM (_VOID, lime_curl_global_cleanup, _NO_ARG);
	DEFINE_HL_PRIM (_I32, lime_curl_global_init, _I32);
	DEFINE_HL_PRIM (_I32, lime_curl_multi_cleanup, _TCFFIPOINTER);
	DEFINE_HL_PRIM (_TCFFIPOINTER, lime_curl_multi_init, _NO_ARG);
	DEFINE_HL_PRIM (_I32, lime_curl_multi_add_handle, _TCFFIPOINTER _TCFFIPOINTER);
	DEFINE_HL_PRIM (_I32, lime_curl_multi_get_running_handles, _TCFFIPOINTER);
	DEFINE_HL_PRIM (_DYN, lime_curl_multi_info_read, _TCFFIPOINTER _DYN);
	DEFINE_HL_PRIM (_I32, lime_curl_multi_perform, _TCFFIPOINTER);
	DEFINE_HL_PRIM (_I32, lime_curl_multi_remove_handle, _TCFFIPOINTER _TCFFIPOINTER);
	DEFINE_HL_PRIM (_I32, lime_curl_multi_wait, _TCFFIPOINTER _I32);
	DEFINE_HL_PRIM (_BYTES, lime_curl_version, _NO_ARG);
	DEFINE_HL_PRIM (_DYN, lime_curl_version_info, _I32);
	
	
}


extern "C" int lime_curl_register_prims () {
	
	return 0;
	
}