using System.Collections;
using Mercive.Input;
using Mercive.Playback;
using UnityEngine;

namespace Mercive.Beatcaster {
    [RequireComponent(typeof(CapsuleCollider))]
    [RequireComponent(typeof(LineRenderer))]
    public class LinePatternLine : MonoBehaviour {
        public LinePatternControllerEffects GrindEmitter;
        private bool _isTouchingLine;
        private Coroutine _vibrateCoroutine;
        private CapsuleCollider _collider;

        public LinePatternManager Owner { get; set; }

        public LineRenderer LineRenderer { get; private set; }

        public Vector3 StartNote { get; set; }
        public Vector3 EndNote { get; set; }

        public Vector3 StartNoteStart { get; set; }
        public Vector3 EndNoteStart { get; set; }
        public Vector3 StartNoteEnd { get; set; }
        public Vector3 EndNoteEnd { get; set; }
        public float StartNoteTime = 0.0f;
        public float EndNoteTime = 0.0f;
        public float ETA { get; set; }

        public AudioSource AudioSourceOwner { get; set; }

        private bool hasShownFeedback = false;


        public Vector3 EndPos { get; set; }

        private float _lifeTime = 5.0f;


        private void Start () {
            LineRenderer = GetComponent<LineRenderer>();
            _collider = GetComponent<CapsuleCollider>();

            LineRenderer.useWorldSpace = false;
            PlaybackManager.Instance.Playhead.Subdivision += HandleSubdivision;
        }

        private void OnDestroy() {
            PlaybackManager.Instance.Playhead.Subdivision -= HandleSubdivision;
        }

        public bool FlipBit;
        private void Update() {
            // Update line end points
            _lifeTime -= Time.deltaTime;
            if (_lifeTime < 0.0f) {
                Destroy(gameObject);
            }



            if (StartNote != Vector3.zero) {
                //StartNoteTime += Time.deltaTime;
                StartNote = Vector3.LerpUnclamped(StartNoteStart, StartNoteEnd, (AudioSourceOwner.time - StartNoteTime) / ETA);
                transform.position = Vector3.LerpUnclamped(StartNoteStart, StartNoteEnd, (AudioSourceOwner.time - StartNoteTime) / ETA);

                //transform.position = StartNote.transform.position;

                // Set the first vert to this note's position
                LineRenderer.SetPosition(0,
                    transform.InverseTransformPoint(StartNote));


                if (EndNote != Vector3.zero) {
                    //EndNoteTime += Time.deltaTime;
                    EndNote = Vector3.LerpUnclamped(EndNoteStart,EndNoteEnd, (AudioSourceOwner.time - EndNoteTime) / ETA);

                    // Set the second vert to this note's position
                    transform.LookAt(EndNote);
                    //LineRenderer.SetPosition(1,
                    //    transform.InverseTransformPoint(EndNote.transform.position));

                    float length = (LineRenderer.GetPosition(LineRenderer.positionCount - 1) -
                                    LineRenderer.GetPosition(0)).magnitude;
                    Vector3 colliderCenter = _collider.center;
                    colliderCenter.z = length / 2.0f;

                    _collider.center = colliderCenter;
                    _collider.height = length;

                    //pos = Vector3.Lerp(start, end, i / (float) _lightningLineRenderer.positionCount);
                    Vector3 pos;
                    Vector3 start =
                        transform.InverseTransformPoint(StartNote);
                    Vector3 end = transform.InverseTransformPoint(EndNote);
                    Vector3 curveOffsetControlPosition =
                        Vector3.Lerp(start, end, 0.5f) +
                        (FlipBit ? transform.right * 0.2f : -transform.right * 0.2f);
                    for (int i = 1; i < LineRenderer.positionCount; ++i) {

                        float t = i / (float) LineRenderer.positionCount;

                        pos = new Vector3(
                            (1.0f - t) * (1.0f - t) * start.x +
                            2 * (1 - t) * t * curveOffsetControlPosition.x + t * t * end.x,
                            (1.0f - t) * (1.0f - t) * start.y +
                            2 * (1 - t) * t * curveOffsetControlPosition.y + t * t * end.y,
                            (1.0f - t) * (1.0f - t) * start.z +
                            2 * (1 - t) * t * curveOffsetControlPosition.z + t * t * end.z);
                        //LineRenderer.SetPosition(i, start + (i / (float) (LineRenderer.positionCount - 1)) * (end - start));
                        LineRenderer.SetPosition(i, pos);
                    }

                    //transform.SetParent(EndNote.transform);
                }

            }

            if (EndPos == Vector3.zero) {
                Destroy((gameObject));
            }

            if (EndNote == Vector3.zero && EndPos != Vector3.zero) {
                // Set the second vert to this note's position
                transform.LookAt(EndPos);
                //LineRenderer.SetPosition(1,
                //    transform.InverseTransformPoint(EndNote.transform.position));

                float length = (LineRenderer.GetPosition(LineRenderer.positionCount - 1) -
                                LineRenderer.GetPosition(0)).magnitude;
                Vector3 colliderCenter = _collider.center;
                colliderCenter.z = length / 2.0f;

                _collider.center = colliderCenter;
                _collider.height = length;
                Vector3 pos;
                Vector3 start =
                    transform.InverseTransformPoint(StartNote);
                Vector3 end = transform.InverseTransformPoint(EndPos);;
                Vector3 curveOffsetControlPosition =
                    Vector3.Lerp(start, end, 0.5f) +
                    (FlipBit ? transform.right * 0.2f : -transform.right * 0.2f);
                for (int i = 1; i < LineRenderer.positionCount; ++i) {

                    float t = i / (float) LineRenderer.positionCount;

                    pos = new Vector3(
                        (1.0f - t) * (1.0f - t) * start.x +
                        2 * (1 - t) * t * curveOffsetControlPosition.x + t * t * end.x,
                        (1.0f - t) * (1.0f - t) * start.y +
                        2 * (1 - t) * t * curveOffsetControlPosition.y + t * t * end.y,
                        (1.0f - t) * (1.0f - t) * start.z +
                        2 * (1 - t) * t * curveOffsetControlPosition.z + t * t * end.z);
                    //LineRenderer.SetPosition(i, start + (i / (float) (LineRenderer.positionCount - 1)) * (end - start));
                    LineRenderer.SetPosition(i, pos);
                }
            }



//            if (StartNote != null) {
//                if (!StartNote.gameObject.activeSelf) {
//                    StartNote = null;
//                }
//            }

//            else {
//                if (EndNote != null) {
//                    transform.position = EndNote.transform.position;
//                }
//            }
//            if (EndNote != null) {
//                if (!EndNote.gameObject.activeSelf) {
//                    Destroy(gameObject);
//                }
//            }

            // Disable collision detection for other players
            if (!Owner.IsInteractable) {
                return;
            }

            //bool containsLeft = _collider.bounds.Contains(InputLayer.LeftHand.Position);
            //bool containsRight = _collider.bounds.Contains(InputLayer.RightHand.Position);
            //if (!_isTouchingLine && (containsLeft || containsRight)) {
            //    _isTouchingLine = true;

            //    if (_vibrateCoroutine != null) {
            //        StopCoroutine(_vibrateCoroutine);
            //    }
            //    _vibrateCoroutine = StartCoroutine(VibrateOnLine());

            //    if (GrindEmitter != null) {
            //        GrindEmitter.transform.position = containsLeft
            //            ? InputLayer.LeftHand.Position
            //            : InputLayer.RightHand.Position;

            //        Owner.GrindEmitter.Activate();
            //    }
            //}
            //else if (_isTouchingLine && !containsLeft && !containsRight) {
            //    _isTouchingLine = false;

            //    if (_vibrateCoroutine != null) {
            //        StopCoroutine(_vibrateCoroutine);
            //    }

            //    if (GrindEmitter != null) {
            //        Owner.GrindEmitter.Deactivate();
            //    }
            //}
        }


        //public void OnTriggerEnter(Collider other) {
        //    if (!Owner.IsInteractable) {
        //        return;
        //    }

        //    //if (TimeElapsed < ETA) {
        //    //    return;
        //    //}

        //    // Faster to check collider equality than go through the input layer
        //    if (other.transform == Owner.Player.LeftHand ||
        //        other.transform == Owner.Player.RightHand) {

        //        ITrackedHand controller = InputLayer.GetHand(HandRole.Controller);
        //        if (other.transform == Owner.Player.GetHand(controller.Hand)) {
        //            controller.Vibrate(0.3f, 1.0f);

        //            ShowHitFeedback(controller);
        //            //Owner.ReportNoteFeedback(this, NoteFeedbackType.Hit);
        //        }
        //    }
        //}

        [SerializeField] private AudioSource _audioSource;
        [SerializeField] private AudioClip _hitSound, _dragSound;


        private Collider _handCollider;

        public void HandleSubdivision(int i, int j) {
            ITrackedHand controller = InputLayer.GetHand(HandRole.Controller);
            if (_handCollider != null && _handCollider.transform ==
                Owner.Player.GetHand(controller.Hand)) {

                ShowDragFeedback(controller);
                //Owner.ReportNoteFeedback(this, NoteFeedbackType.Hit);
            }

        }
        public void OnTriggerStay(Collider other) {
            if (!Owner.IsInteractable)
                return;

            if (other.transform == Owner.Player.LeftHand ||
                other.transform == Owner.Player.RightHand) {
                _handCollider = other;
                if (Owner.GrindEmitter != null && !hasShownFeedback) {
                    hasShownFeedback = true;
                    Owner.GrindEmitter.ActivateChainEffects();
                }
            }
        }

        public void OnTriggerExit(Collider other) {
            _handCollider = null;
            if (Owner.GrindEmitter != null && hasShownFeedback) {
                hasShownFeedback = false;
                Owner.GrindEmitter.DeactivateChainEffects();
            }

            //if (_audioSource != null && _audioSource.isPlaying) {
            //    _audioSource.Pause();
            //}
        }

        public void ShowDragFeedback(ITrackedHand controller) {
            if (Owner.GrindEmitter != null) {
                Owner.GrindEmitter.transform.position =
                    Owner.Player.GetHand(controller.Hand).position;
            }

            if (Owner.IsInteractable) {
                Owner.Score.AddScore(5);
                //Owner.Score.TriggerPointsPopup(5, Color.magenta, HandRole.Controller);

                controller.Vibrate(0.1f, 0.2f);

                if (_dragSound != null && _audioSource != null) {
                    _audioSource.PlayOneShot(_dragSound);

                }
            }
        }
        //public void ShowHitFeedback(ITrackedHand controller) {


        //    if (Owner.GrindEmitter != null) {
        //        Owner.GrindEmitter.transform.position =
        //            Owner.Player.GetHand(controller.Hand).position;

        //        Owner.GrindEmitter.Activate();
        //    }

        //    if (Owner.IsInteractable) {

        //        Owner.Score.AddScore(15);
        //        Owner.Score.TriggerPointsPopup(15, Color.magenta, HandRole.Controller);

        //        if (_hitSound != null && _audioSource != null) {
        //            _audioSource.PlayOneShot(_hitSound);

        //        }
        //    }

        //}
    }
}
