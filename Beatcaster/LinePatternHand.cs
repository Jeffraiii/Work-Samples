using Boo.Lang;
using Mercive.Input;
using Mercive.Playback;
using UnityEngine;

namespace Mercive.Beatcaster {
    [RequireComponent(typeof(AudioSource))]
    public class LinePatternHand : LinePatternNoteBase {
        [SerializeField] private float _failAfterEtaDuration = 0.3f;
        [SerializeField] private ParticleSystem _hitEmitter;
        [SerializeField] private Collider _absorptionZone;
        [SerializeField] private GameObject _handNoteParticleSystem;
        [SerializeField] private AudioClip _hitSound;
        [SerializeField] private float _pitchRange = 0.1f;

        private bool _isTouched = false;
        private float _destroyTimer = 0.5f;
        public SpriteRenderer[] _spriteRenderers;

        private bool _hasShownFeedback;

        private readonly int[] _beatmap = { 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1 };
        private int _conductor;

        public HandGesture Gesture { get; set; }

        protected override void Start() {
            base.Start();
            //transform.position = Vector3.down;
            //_audioSource = GetComponent<AudioSource>();

            // Flip scale for lefties
            if (Owner.Player != null && Owner.Player.DominantHand == Handedness.Left) {
                Vector3 scale = transform.localScale;
                scale.x *= -1;
                transform.localScale = scale;
            }

            PlaybackManager.Instance.Playhead.Subdivision += HandleSubdivision;
        }

        public override void ResetState() {
            base.ResetState();
            _hasShownFeedback = false;
            //transform.position = Vector3.down;
            _conductor = 0;
        }

        private void OnDestroy() {
            if (PlaybackManager.HasInstance) {
                PlaybackManager.Instance.Playhead.Subdivision -= HandleSubdivision;
            }
        }

        [SerializeField] private float _percentToAppear = 0.5f;
        private void Update() {
            if (_isTouched) {
                _destroyTimer -= Time.deltaTime;
                if (_destroyTimer < 0.0f) {
                    _isTouched = false;
                    _destroyTimer = 0.5f;
                    Owner.DestroyNote(this);
                }
                return;
            }
            float t = TimeElapsed / ETA;
            Vector3 pos = Vector3.LerpUnclamped(StartPosition, EndPosition, t);
            //pos.y = (((1f * (t - _percentToAppear)) / (0.04f + Mathf.Abs(t - _percentToAppear))) + 0f) *
            //        StartPosition.y; //change 1f, 0f to 0.5,0.5 for 0 to 1 value

            transform.position = pos;

            if (TimeElapsed > ETA + Lifetime) {
                _hasShownFeedback = true;
                Owner.DestroyNote(this);
                return;
            }

            //if (TimeElapsed >= ETA && _canHitsound && !_hitSounded) {
            //    _audioSource.PlayOneShot(_hitSound);
            //    _hitSounded = true;
            //}

            if (!Owner.IsInteractable) {
                return;
            }

            if (!_hasShownFeedback && TimeElapsed > ETA + _failAfterEtaDuration) {
                _hasShownFeedback = true;
                ShowFailFeedback();
                Owner.ReportNoteFeedback(this, NoteFeedbackType.Missed);
            }
        }

        private void OnTriggerStay(Collider other) {
            if (!Owner.IsInteractable || _hasShownFeedback) {
                return;
            }

            // Faster to check collider equality than go through the input layer
            if (other.transform == Owner.Player.LeftHand ||
                other.transform == Owner.Player.RightHand) {

                ITrackedHand hand = InputLayer.GetHand(HandRole.Hand);
                if (other.transform == Owner.Player.GetHand(hand.Hand)) {
                    _hasShownFeedback = true;
                    ShowHitFeedback();
                    Owner.ReportNoteFeedback(this, NoteFeedbackType.Hit);
                }
            }
        }

        public override void ShowHitFeedback() {
            if (Owner.GrindEmitter2 != null) {
                Owner.GrindEmitter2.transform.position = transform.position;

                if (Owner.IsFreestyle) {
                    Owner.GrindEmitter2.BurstFreestyleSparkle(30);
                } else {
                    Owner.GrindEmitter2.BurstSparkle(6);
                }
            }

            if (Owner.IsInteractable) {
                Owner.Score.AddScore(100);

                Owner.Score.TriggerPointsPopup(100, Color.yellow, HandRole.Hand);

                if (_hitSound != null) {
                    float pitch = Random.Range(1 - _pitchRange, 1 + _pitchRange);

                    if (TimeElapsed >= ETA) {
                        Owner.AudioSource.Play(_hitSound,
                            pitch: pitch);
                    } else {
                        Owner.AudioSource.Play(_hitSound,
                            pitch: pitch,
                            scheduledTime: AudioSettings.dspTime + ETA - TimeElapsed);
                    }
                }
            }

            _isTouched = true;
            foreach (var sprite in _spriteRenderers) {
                sprite.enabled = false;
            }
            //Owner.DestroyNote(this);
        }

        public override void ShowFailFeedback() {
        }

        private void HandleSubdivision(int i, int j) {
//            if (_containsHand && _beatmap[_conductor % _beatmap.Length] > 0) {
//                _audioSource.Play();
//            }

            _conductor = (_conductor + 1) % _beatmap.Length;
        }
    }
}
