using Mercive.Input;
using UnityEngine;
using System.Collections.Generic;

namespace Mercive.Beatcaster {
    [RequireComponent(typeof(AudioSource))]
    [RequireComponent(typeof(Collider))]
    public class LinePatternNote : LinePatternNoteBase {
        [SerializeField] private float _failAfterEtaDuration = 0.3f;
        [SerializeField] private Collider _absorptionZone;
        [SerializeField] private Transform _noteVisual;

        [SerializeField] private ParticleSystem _hitEmitter;
        [SerializeField] private ParticleSystem _missEmitter;
        [SerializeField] private AudioClip _failSound;
        [SerializeField] private AudioClip[] _hitSound;
        [SerializeField] private float _pitchRange = 0.1f;

        private bool _hasShownFeedback;

        protected override void Start() {
            base.Start();
            //transform.position = Vector3.down;
            //if (_noteHitbox == null) _noteHitbox = GetComponent<CollisionListener>();
            if(_noteHitbox != null) _noteHitbox.TriggerStay.AddListener(OnTriggerInChildStayed);
            //_audioSource = GetComponent<AudioSource>();
        }


        public override void ResetState() {
            base.ResetState();
            //transform.position = Vector3.down;
            _hasShownFeedback = false;
        }

        [SerializeField] private float _percentToAppear = 0.5f;

        public float PercentToAppear => _percentToAppear;

        private void Update() {
            //fast sigmoid: x / (1 + abs(x))
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

            if (!Owner.IsInteractable) {
                return;
            }

            //if (TimeElapsed >= ETA && _canHitsound && !_hitSounded) {
            //    _audioSource.PlayOneShot(_hitSound[Playback.PlaybackManager.Instance.Playhead.CurrentSubdivision % _hitSound.Count]);
            //    _hitSounded = true;
            //}

            if (!_hasShownFeedback && TimeElapsed > ETA + _failAfterEtaDuration) {
                _hasShownFeedback = true;
                ShowFailFeedback();
                Owner.ReportNoteFeedback(this, NoteFeedbackType.Missed);
            }

            bool hasLine = false;
            foreach (var line in gameObject.GetComponentsInChildren<LinePatternLine>()) {
                if (line.gameObject.activeSelf && line.EndNote != null && line.StartNote != null) {
                    if (!hasLine) {
                        hasLine = true;
                    } else {
                        Destroy(line.gameObject);
                    }
                }
            }
            //if (GetComponentInChildren<LinePatternLine>()?.StartNote == this) {
            //    Destroy(GetComponentInChildren<LinePatternLine>() ?.gameObject);
            //}
        }

        [SerializeField] private CollisionListener _noteHitbox;

        public void OnTriggerInChildStayed(Collider other) {
            if (!Owner.IsInteractable || _hasShownFeedback) {
                return;
            }

            // Faster to check collider equality than go through the input layer
            if (other.transform == Owner.Player.LeftHand ||
                other.transform == Owner.Player.RightHand) {

                ITrackedHand controller = InputLayer.GetHand(HandRole.Controller);
                if (other.transform == Owner.Player.GetHand(controller.Hand)) {
                    controller.Vibrate(0.3f, 0.5f);

                    _hasShownFeedback = true;
                    ShowHitFeedback();
                    Owner.ReportNoteFeedback(this, NoteFeedbackType.Hit);
                }
            }
        }

        public override void ShowHitFeedback() {
            if (_hitEmitter != null) {
                _hitEmitter.Emit(1);
            }

            if (Owner.GrindEmitter != null) {
                Owner.GrindEmitter.transform.position = transform.position;

                if (Owner.IsFreestyle) {
                    Owner.GrindEmitter.BurstFreestyleSparkle(30);
                } else {
                    Owner.GrindEmitter.BurstSparkle(6);
                }
            }

            if (Owner.IsInteractable) {
                Owner.Score.AddScore(25);
                Owner.Score.TriggerPointsPopup(25, Color.cyan, HandRole.Controller);

                if (_hitSound != null) {
                    if (TimeElapsed >= ETA) {
                        Owner.AudioSource.PlayRandom(_hitSound,
                            pitchRange: _pitchRange);
                    } else {
                        Owner.AudioSource.PlayRandom(_hitSound,
                            pitchRange: _pitchRange,
                            scheduledTime: AudioSettings.dspTime + ETA - TimeElapsed);
                    }
                }
            }

            Owner.DestroyNote(this);
        }

        public override void ShowFailFeedback() {
            if (Owner.IsInteractable) {
                if (_missEmitter != null) {
                    _missEmitter.time = 0;
                    _missEmitter.Play();
                }

                Owner.Score.AddScore(0);

                if (_failSound != null) {
                    Owner.AudioSource.Play(_failSound);
                }
            }
        }
    }
}
