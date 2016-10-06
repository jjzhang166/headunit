

int hu_aap_mic_get();
int hu_aap_out_get(int chan);

#ifdef __cplusplus
extern "C" {
#endif

int hu_aap_tra_recv(byte *buf, int len, int tmo); // Used by intern, hu_ssl
int hu_aap_tra_set(int chan, int flags, int type, byte *buf,
                   int len); // Used by intern                       hu_ssl
int hu_aap_tra_send(int retry, byte *buf, int len,
                    int tmo); // Used by intern,                      hu_ssl
int hu_aap_enc_send(
    int retry, int chan, byte *buf,
    int len);      // Used by intern,            hu_jni     // Encrypted Send
int hu_aap_stop(); // Used by          hu_mai,  hu_jni     // NEED: Send Byebye,
                   // Stops USB/ACC/OAP
int hu_aap_start(byte ep_in_addr, byte ep_out_addr, long myip_string,
                 int transport_audio, int hr); // Used by          hu_mai,
                                               // hu_jni     // Starts
                                               // USB/ACC/OAP, then AA protocol
                                               // w/ VersReq(1), SSL handshake,
                                               // Auth Complete
int hu_aap_recv_process(); // Used by          hu_mai,  hu_jni     // Process 1
                           // encrypted receive message set:

#ifdef __cplusplus
}
#endif

#define sd_buf_aud_len 2 + 4 + 6 + 8 + 2 + 4 + 6 + 7 + 2 + 4 + 6 + 7 // 58
// Channels ( or Service IDs)
#define AA_CH_CTR                                                              \
  0 // Sync with hu_tra.java, hu_aap.h and hu_aap.c:aa_type_array[]
#define AA_CH_SEN 1
#define AA_CH_VID 2
#define AA_CH_TOU 3
#define AA_CH_AUD 4
#define AA_CH_AU1 5
#define AA_CH_AU2 6
#define AA_CH_MIC 7
#define AA_CH_MAX 7

// ADDED: buffer functions' prototype
int aa_pro_ctr_a00(int chan, byte *buf, int len);
int aa_pro_ctr_a01(int chan, byte *buf, int len);
int aa_pro_ctr_a02(int chan, byte *buf, int len);
int aa_pro_ctr_a03(int chan, byte *buf, int len);
int aa_pro_ctr_a04(int chan, byte *buf, int len);
int aa_pro_ctr_a05(int chan, byte *buf, int len);
int aa_pro_ctr_a06(int chan, byte *buf, int len);
int aa_pro_ctr_a08(int chan, byte *buf, int len);
int aa_pro_ctr_a09(int chan, byte *buf, int len);
int aa_pro_ctr_a0a(int chan, byte *buf, int len);
int aa_pro_ctr_a0b(int chan, byte *buf, int len);
int aa_pro_ctr_a0c(int chan, byte *buf, int len);
int aa_pro_ctr_a0d(int chan, byte *buf, int len);
int aa_pro_ctr_a0e(int chan, byte *buf, int len);
int aa_pro_ctr_a0f(int chan, byte *buf, int len);
int aa_pro_ctr_a10(int chan, byte *buf, int len);
int aa_pro_ctr_a11(int chan, byte *buf, int len);
int aa_pro_ctr_a12(int chan, byte *buf, int len);
int aa_pro_ctr_a13(int chan, byte *buf, int len);
int aa_pro_ctr_a14(int chan, byte *buf, int len);
int aa_pro_ctr_a15(int chan, byte *buf, int len);
int aa_pro_ctr_a16(int chan, byte *buf, int len);
int aa_pro_ctr_a17(int chan, byte *buf, int len);
int aa_pro_ctr_a18(int chan, byte *buf, int len);
int aa_pro_ctr_a19(int chan, byte *buf, int len);
int aa_pro_ctr_a1a(int chan, byte *buf, int len);
int aa_pro_ctr_a1b(int chan, byte *buf, int len);
int aa_pro_ctr_a1c(int chan, byte *buf, int len);
int aa_pro_ctr_a1d(int chan, byte *buf, int len);
int aa_pro_ctr_a1e(int chan, byte *buf, int len);
int aa_pro_ctr_a1f(int chan, byte *buf, int len);
int aa_pro_all_a07(int chan, byte *buf, int len);
int aa_pro_snk_b00(int chan, byte *buf, int len);
int aa_pro_aud_b01(int chan, byte *buf, int len);
int hu_aap_out_get(int chan);
int aa_pro_aud_b02(int chan, byte *buf, int len);
int aa_pro_vid_b01(int chan, byte *buf, int len);
int aa_pro_vid_b07(int chan, byte *buf, int len);
int aa_pro_sen_b01(int chan, byte *buf, int len);
int aa_pro_tou_b02(int chan, byte *buf, int len);
int hu_aap_mic_get();
int aa_pro_mic_b01(int chan, byte *buf, int len);
int aa_pro_mic_b04(int chan, byte *buf, int len);
int aa_pro_mic_b05(int chan, byte *buf, int len);
