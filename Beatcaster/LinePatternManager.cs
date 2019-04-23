using System.Collections.Generic;
using UnityEngine;
using Mercive.Playback;
using System.Linq;
using DG.Tweening;
using Mercive.Input;
using Photon.Pun;

namespace Mercive.Beatcaster {
    public enum NoteFeedbackType {
        Missed,
        Hit
    }

    [RequireComponent(typeof(VoicedAudioSource))]
    public class LinePatternManager : MonoBehaviourPun, IPunObservable {
        public class LineAttribute {
            public LinePatternLine Line;
            public bool HasPrevious, IsUsedThisBeat;

            public LineAttribute() {
                HasPrevious = false;
                IsUsedThisBeat = false;

            }
            public LineAttribute(LinePatternLine line) {
                Line = line;
                HasPrevious = false;
                IsUsedThisBeat = false;
            }
        }

        [System.Serializable]
        private struct BeatmapList {
            public string[] Beatmaps;
        }

        public const int GridX = 4;
        public const int GridY = 4;
        private const float PhraseOffsetTime = -0.01f;
        private const int DefaultNotePoolCount = 10;

        [SerializeField] private BeatmapList[] _songBeatmapsToLoad = new BeatmapList[0];

        [SerializeField] private LinePatternNote _notePrefab;
        [SerializeField] private LinePatternHand _handNotePrefab;

        [SerializeField] private Transform _laneStart;
        [SerializeField] private Transform _laneEnd;

        [SerializeField] private float _globalNoteOffset = 0.0f;
        [SerializeField] private float _globalGridScaleX = 0.5f;
        [SerializeField] private float _globalGridScaleY = 0.5f;

        [SerializeField] private LinePatternLine _lineAssetLeft;
        [SerializeField] private LinePatternLine _lineAssetRight;
        public LinePatternControllerEffects GrindEmitter, GrindEmitter2;

        [SerializeField] private List<Transform> _noteSpawners;

        [SerializeField] private LinePatternScore _score;
        [SerializeField] private LinePatternScoreboard _scoreboard;

        /// A map of note types to the last line for each note (prev _linePool)
        private readonly Dictionary<int, LineAttribute> _currentLines =
            new Dictionary<int, LineAttribute>();

        private readonly Dictionary<int, LinePatternNoteBase> _activeNotes = new Dictionary<int, LinePatternNoteBase>(DefaultNotePoolCount * 2);

        private BasicUnityObjectPool<LinePatternNote> _notePool;
        private BasicUnityObjectPool<LinePatternHand> _handNotePool;

        private int _currentDifficulty = 0;
        private bool flipbit = false;
        private bool _isMuted;

        private int _streamValue = 0; //0 = no change, 1 = up, -1 = down
        private int _currentNoteId = 1;

        //1: single note
        //2-17: chained note. If note occurs in consecutive pattern, draw a line between them
        public List<Beatmap> Beatmaps { get; } = new List<Beatmap>();

        public Beatmap CurrentBeatmap => Beatmaps[_currentDifficulty % Beatmaps.Count];

        public Player Player { get; internal set; }

        /// 0 for no player, 1+ if associated with a player
        public int PlayerNumber => Player == null ? 0 : Player.PlayerId;

        /// Whether this LinePatternManager can interact with a player or whether it just plays through without feedback
        public bool IsInteractable => Player == null || Player.IsLocalPlayer;

        /// Whether the grid is flipped on the x axis
        public bool IsGridFlipped { get; set; }

        public bool IsFreestyle { get; private set; }

        public LinePatternScore Score => _score;
        public LinePatternScoreboard Scoreboard => _scoreboard;
        public float GlobalNoteOffset => _globalNoteOffset;
        public float GlobalGridScaleX => _globalGridScaleX;
        public float GlobalGridScaleY => _globalGridScaleY;

        public VoicedAudioSource AudioSource { get; private set; }

        public event System.Action<int> BeatmapLoaded;
        public event System.Action DifficultyChanged;

        private List<List<int>> _endChainNotes = new List<List<int>>();
        private List<List<Vector2Int>> _chainPosMaps = new List<List<Vector2Int>>();
        private int difficulty = 0;
        private int _endChainIndex = 0;

        private Quaternion _grindEmitterStartRotation1, _grindEmitterStartRotation2;

        [SerializeField] private LineRenderer _noteSpawnLine, _handSpawnLine;
        [SerializeField] private Transform _lightningOrigin;
        [SerializeField] private ParticleSystem _lightningOriginPS, _noteSpawnPS, _handSpawnPS;
        [SerializeField] private float _noiseIntensity = 0.01f;

        System.Collections.IEnumerator LightningStrikeCr(LineRenderer lr, ParticleSystem endPS, Vector3 start,
            Vector3 curve, Vector3 end, float duration) {
            lr.enabled = true;
            DrawLightningStrike(lr, _lightningOriginPS, endPS, start, curve, end);
            yield return new WaitForSeconds(duration / 3.0f);
            DrawLightningStrike(lr, _lightningOriginPS, endPS, start, curve, end);
            yield return new WaitForSeconds(duration / 3.0f);
            DrawLightningStrike(lr, _lightningOriginPS, endPS, start, curve, end);
            yield return new WaitForSeconds(duration / 3.0f);

            lr.enabled = false;
        }
        public void DrawLightningStrike(LineRenderer _lightningLineRenderer, ParticleSystem startPS, ParticleSystem endPS, Vector3 start, Vector3 curveOffsetControlPosition, Vector3 end) {
            Vector3 pos;
            if (startPS != null) {
                startPS.transform.position = end;
                startPS.time = 0;
                startPS.Play();
            }

            if (endPS != null) {
                endPS.transform.position = start;
                endPS.time = 0;
                endPS.Play();
            }

            for (int i = 0; i < _lightningLineRenderer.positionCount; ++i) {
                float t = i / (float) _lightningLineRenderer.positionCount;
                //pos = Vector3.Lerp(start, end, i / (float) _lightningLineRenderer.positionCount);
                pos = new Vector3(
                    (1.0f - t) * (1.0f - t) * start.x + 2 * (1 - t) * t * curveOffsetControlPosition.x + t * t * end.x,
                    (1.0f - t) * (1.0f - t) * start.y + 2 * (1 - t) * t * curveOffsetControlPosition.y + t * t * end.y,
                    (1.0f - t) * (1.0f - t) * start.z + 2 * (1 - t) * t * curveOffsetControlPosition.z + t * t * end.z);

                if (i != 0 && i != _lightningLineRenderer.positionCount - 1) {
                    pos += Random.insideUnitSphere * _noiseIntensity; // a random offset
                }

                _lightningLineRenderer.SetPosition(i, pos);
            }
        }
        private void Start() {
            AudioSource = GetComponent<VoicedAudioSource>();

            _grindEmitterStartRotation1 = GrindEmitter.transform.rotation;
            _grindEmitterStartRotation2 = GrindEmitter2.transform.rotation;
            // Create the note pools
            _notePool = new BasicUnityObjectPool<LinePatternNote>(_notePrefab, transform, DefaultNotePoolCount);
            _handNotePool = new BasicUnityObjectPool<LinePatternHand>(_handNotePrefab, transform, DefaultNotePoolCount);

            PlaybackManager.Instance.NewSong += OnNewSong;
            if (_songBeatmapsToLoad.Length > 0) {
                OnNewSong(0, null);
            }
        }

        private void Update() {
            if (IsFreestyle) {
                if (GrindEmitter != null) {
                    GrindEmitter.transform.position = Player != null ? Player.LeftHand.position : InputLayer.LeftHand.Position;
                    GrindEmitter.transform.rotation = Player != null ? Player.LeftHand.rotation : InputLayer.LeftHand.Rotation;
                }

                if (GrindEmitter2 != null) {
                    GrindEmitter2.transform.position = Player != null ? Player.RightHand.position : InputLayer.RightHand.Position;
                    GrindEmitter2.transform.rotation = Player != null ? Player.RightHand.rotation : InputLayer.RightHand.Rotation;
                }
            }

            if (!IsFreestyle) {
                if (GrindEmitter != null) GrindEmitter.transform.rotation = _grindEmitterStartRotation1;

                if (GrindEmitter2 != null)
                    GrindEmitter2.transform.rotation = _grindEmitterStartRotation2;
            }
        }

        private void OnDrawGizmosSelected() {
            if (_laneStart == null || _laneEnd == null) {
                return;
            }

            Gizmos.color = Color.blue;
            Gizmos.DrawLine(GetGridStartPosition(0, 0), GetGridEndPosition(0, 0));
            Gizmos.DrawLine(GetGridStartPosition(GridX - 1, 0), GetGridEndPosition(GridX - 1, 0));
            Gizmos.DrawLine(GetGridStartPosition(GridX - 1, GridY - 1), GetGridEndPosition(GridX - 1, GridY - 1));
            Gizmos.DrawLine(GetGridStartPosition(0, GridY - 1), GetGridEndPosition(0, GridY - 1));

            Gizmos.color = Color.cyan;
            for (int x = 0; x < GridX; ++x) {
                Gizmos.DrawLine(GetGridEndPosition(x, 0), GetGridEndPosition(x, GridY - 1));
            }
            for (int y = 0; y < GridY; ++y) {
                Gizmos.DrawLine(GetGridEndPosition(0, y), GetGridEndPosition(GridX - 1, y));
            }
        }

        public void OnPhotonSerializeView(PhotonStream stream, PhotonMessageInfo info) {
            if (stream.IsWriting) {
                stream.SendNext(_currentDifficulty);
                stream.SendNext(IsFreestyle);
            } else {
                int newCurrentDifficulty = (int) stream.ReceiveNext();
                if (newCurrentDifficulty != _currentDifficulty) {
                    _currentDifficulty = newCurrentDifficulty;

                    // Same as ExecuteStreamSwitch()
                    DifficultyChanged?.Invoke();
                }

                bool newIsFreestyle = (bool) stream.ReceiveNext();
                ChangeFreestyleMode(newIsFreestyle);
            }
        }

        private void OnNewSong(int newSongIndex, SongData _) {
            Beatmaps.Clear();
            _endChainNotes.Clear();
            _chainPosMaps.Clear();
            difficulty = 0;
            foreach (string path in _songBeatmapsToLoad[newSongIndex].Beatmaps) {
                LoadMap(Beatmap.Load(path));
                difficulty++;
            }
        }

        public void LoadMap(Beatmap newMap) {
            Beatmaps.Add(newMap);
            int mapDifficulty = difficulty;
            List<int> tempMap = new List<int>();
            List<Vector2Int> tempPosMap = new List<Vector2Int>();
            tempMap.Add(-1);
            for (int i = 0; i < newMap.BeatmapNotes.Count; ++i) {
                var pattern = newMap.BeatmapNotes.ElementAt(i);
                int iCopy = i; // save a copy for use in closure

                bool hasPushed = false;
                for (int j = 0; j < pattern.Value.GetLength(0); ++j)
                {
                    for (int k = 0; k < pattern.Value.GetLength(1); ++k) {
                        if (pattern.Value[k, j] == 2) {
                            if (tempMap.Last() == 3) {
                                tempMap.RemoveAt(tempMap.Count - 1);
                                tempMap.Add(0);
                            }
                            else if (tempMap.Last() == 5) {
                                for (int index = tempMap.Count - 1; index >= 0; index--) {
                                    if (tempMap[index] != 5) {
                                        if (tempMap[index] == 3) {
                                            tempMap[index] = 0;
                                        }
                                        break;
                                    }
                                }
                            }
                            tempMap.Add(2);
                            tempPosMap.Add(new Vector2Int(k, j));
                            hasPushed = true;
                        }
                        else if (pattern.Value[k, j] == 3) {
                            if (tempMap.Last() == 2) {
                                tempMap.RemoveAt(tempMap.Count - 1);
                                tempMap.Add(0);
                            }
                            else if (tempMap.Last() == 5) {
                                for (int index = tempMap.Count - 1; index >= 0; index--) {
                                    if (tempMap[index] != 5) {
                                        if (tempMap[index] == 2) {
                                            tempMap[index] = 0;
                                        }
                                        break;
                                    }
                                }
                            }
                            tempMap.Add(3);
                            tempPosMap.Add(new Vector2Int(k, j));
                            hasPushed = true;
                        }

                    }
                }
                if (!hasPushed) {
//                    if (tempMap.Last() == 2 || tempMap.Last() == 3) {
//                        tempMap.RemoveAt(tempMap.Count - 1);
//                        tempMap.Add(0);
//                    }
                    tempMap.Add(5);
                    tempPosMap.Add(new Vector2Int(-1, -1));
                }

                PlaybackManager.Instance.Playhead.Subscribe(
                    () => InstantiateNotePattern(iCopy, mapDifficulty, pattern.Value),
                    pattern.Key.Beat, pattern.Key.Subdivision, _globalNoteOffset);
            }
            tempMap.RemoveAt(0);
            tempMap.RemoveAt(tempMap.Count-1);
            tempMap.Add(0);
            _endChainNotes.Add(tempMap);

            _chainPosMaps.Add(tempPosMap);

            for (int i = 0; i < newMap.BeatmapHandNotes.Count; ++i) {
                var handnote = newMap.BeatmapHandNotes[i];

                for (float offset = 0; offset <= handnote.Length; offset += 0.25f) {
                    PlaybackManager.Instance.Playhead.Subscribe(
                        () => InstantiateHandNote(mapDifficulty, handnote.HorizontalIndex),
                        (int) (handnote.Beat + offset), (int) (handnote.Subdivision + ((offset % 1) * 4) % 4),
                        _globalNoteOffset);
                }
            }

            for (int i = 0; i < newMap.BeatmapSectionMarkers.Count; ++i) {
                var sectionMarker = newMap.BeatmapSectionMarkers[i];
                PlaybackManager.Instance.Playhead.Subscribe(
                    () => EnterNewSection(mapDifficulty, sectionMarker.IsFreestyle, sectionMarker.IsBreak),
                    sectionMarker.Beat, sectionMarker.Subdivision);
            }

            for (int i = 0; i < newMap.BeatmapPhraseMarkers.Count; ++i) {
                var phraseMarker = newMap.BeatmapPhraseMarkers[i];
                PlaybackManager.Instance.Playhead.Subscribe(
                    () => EnterNewPhrase(mapDifficulty, phraseMarker.Flag),
                    phraseMarker.Beat, phraseMarker.Subdivision,
                    _globalNoteOffset + PhraseOffsetTime); // offset the phrases before the notes so that phrases will take effects before notes spawn
            }

            BeatmapLoaded?.Invoke(Beatmaps.Count - 1);
        }

        [SerializeField] private Animator _animator;
        private void InstantiateNotePattern(int beatmapIndex, int mapDifficulty, int[,] map) {
            if (_isMuted || _currentDifficulty != mapDifficulty) {
                return;
            }
            if(_animator != null) _animator.Play("SpawnNote");

            bool onlyHandNote = true;

            for (int i = 0; i < map.GetLength(0); ++i) {
                for (int j = 0; j < map.GetLength(1); ++j) {
                    //instantiate note on the grid
                    if (map[j, i] <= 0) {
                        continue;
                    }

                    if (map[j, i] == 5) {
                        LinePatternHand handNote = _handNotePool.Get();
                        handNote.NoteId = _currentNoteId++;
                        handNote.Owner = this;
                        handNote.AudioSourceOwner = PlaybackManager.Instance.GetAudioSourceForStem(PlaybackManager.Instance.CurrentSong.MainStem);
                        handNote.StartPosition = GetGridStartPosition(i, 2);
                        handNote.EndPosition = GetGridEndPosition(i, 2);
                        handNote.ETA = -_globalNoteOffset;
                        handNote.Gesture = HandGesture.OpenHand;

                        if (_handSpawnLine != null && _lightningOrigin != null)
                            StartCoroutine(LightningStrikeCr(_handSpawnLine, _handSpawnPS, handNote.StartPosition,
                                handNote.StartPosition, _lightningOrigin.position, 0.1f));

                        handNote.ResetState();
                        _activeNotes.Add(handNote.NoteId, handNote);
                        //LinePatternNote freen =
                        //    Instantiate(_freestyleNoteAsset[_currentDifficulty % _noteAsset.Count]);
                        //freen.Owner = this;
                        //freen.StartPosition = GetGridStartPosition(i, j);
                        //freen.transform.position = freen.StartPosition;
                        //freen.EndPosition = GetGridEndPosition(i, j);

                        //freen.transform.rotation = Quaternion.LookRotation(freen.EndPosition - freen.StartPosition);
                        //freen.ETA = -_globalNoteOffset;
                        //freen.AudioSourceOwner = PlaybackManager.Instance.GetAudioSourceForStem(PlaybackManager.Instance.Song.MainStem);
                        continue;
                    }

                    onlyHandNote = false;

                    LinePatternNote note = _notePool.Get();
                    note.NoteId = _currentNoteId++;
                    note.Owner = this;
                    note.AudioSourceOwner = PlaybackManager.Instance.GetAudioSourceForStem(PlaybackManager.Instance.CurrentSong.MainStem);
                    note.StartPosition = GetGridStartPosition(i, j);
                    note.EndPosition = GetGridEndPosition(i, j);
                    note.ETA = -_globalNoteOffset;

                    if (_noteSpawnLine != null && _lightningOrigin != null)
                        StartCoroutine(LightningStrikeCr(_noteSpawnLine, _noteSpawnPS,  note.StartPosition,
                            note.StartPosition, _lightningOrigin.position, 0.1f));

                    note.ResetState();
                    _activeNotes.Add(note.NoteId, note);

                    if (_noteSpawners != null && _noteSpawners.Count > 0) {
                        _noteSpawners[map[j, i] % _noteSpawners.Count].transform
                            .DOMove(note.StartPosition, 0.2f).SetEase(Ease.OutSine);
                    }

                    //if line type, draw the line
                    if (beatmapIndex > 0 && map[j, i] > 1) {
                        // Get the last LineAttribute for this note type
                        LineAttribute previousNote;
                        if (!_currentLines.TryGetValue(map[j, i], out previousNote)) {
                            previousNote = new LineAttribute();
                            _currentLines.Add(map[j, i], previousNote);
                        }

                        // If this is a chain note
                        if (previousNote.HasPrevious) {
                            // Connect this note to the previous line
//                            previousNote.Line.FlipBit = flipbit;
//                            flipbit = !flipbit;
                            previousNote.Line.EndNote = note.transform.position;
                            previousNote.Line.EndNoteStart = note.StartPosition;
                            previousNote.Line.EndNoteEnd = note.EndPosition;
                            previousNote.Line.EndNoteTime = previousNote.Line.AudioSourceOwner.time;

                            previousNote.Line.Owner = this;
                            previousNote.Line.GrindEmitter = GrindEmitter;
                            previousNote.Line.transform.SetParent(note.transform);
                            //previousNote.IsUsedThisBeat = false;

                        }

                        // Create a new line on this note
                        LinePatternLine line = Instantiate(
                            map[j, i] % 2 != 0 ? _lineAssetRight : _lineAssetLeft);

                        line.Owner = this;
                        line.StartNote = note.transform.position;
                        line.StartNoteStart = note.StartPosition;
                        line.StartNoteEnd = note.EndPosition;
                        line.AudioSourceOwner = PlaybackManager.Instance.GetAudioSourceForStem(PlaybackManager.Instance.CurrentSong.MainStem);
                        line.StartNoteTime = line.AudioSourceOwner.time;
                        line.ETA = note.ETA;
                        line.FlipBit = flipbit;
                        flipbit = !flipbit;
                        if (_endChainNotes[mapDifficulty][beatmapIndex] != 0) {
                            while (_chainPosMaps[mapDifficulty][beatmapIndex + 1].y == -1) {
                                beatmapIndex++;
                            }

                            Vector2Int p = _chainPosMaps[mapDifficulty][beatmapIndex + 1];
                            line.EndPos = GetGridStartPosition(p.y, p.x);
                        } else {
                            line.EndPos = Vector3.zero;
                        }
                        previousNote.Line = line;
                        previousNote.HasPrevious = true;
                        previousNote.IsUsedThisBeat = true;
                        _endChainIndex++;

                        foreach (var lineAttribute in _currentLines.Values) {
                            if (!lineAttribute.IsUsedThisBeat) {
                                lineAttribute.HasPrevious = false;
                            }

                            lineAttribute.IsUsedThisBeat = false;
                        }
                    }

                    //else if (!previousNote.HasPrevious){
                    //    //Debug.Log("Key Contained!");
                    //    previousNote.IsUsedThisBeat = true;
                    //    //two endpoints on the line, generate visual & collider
                    //    _linePool[map[j,i]].LR.transform.LookAt(n.transform.position);
                    //   _linePool[map[j,i]].LR.SetPosition(1, previousNote.LR.transform.InverseTransformPoint(n.transform.position));

                    //    var l = (previousNote.LR.GetPosition(1) -
                    //             previousNote.LR.GetPosition(0)).magnitude;
                    //    var c = previousNote.LR.transform
                    //        .GetComponent<CapsuleCollider>().center;
                    //    c.z = (l / 2.0f);

                    //    previousNote.LR.transform
                    //        .GetComponent<CapsuleCollider>().center = c;
                    //    previousNote.LR.transform
                    //        .GetComponent<CapsuleCollider>().height = l;
                    //}
                }
            }


        }

        private void InstantiateHandNote(int mapDifficulty, int horizontalIndex) {
            if (_isMuted || _currentDifficulty != mapDifficulty) {
                return;
            }

            LinePatternHand handNote = _handNotePool.Get();

            handNote.NoteId = _currentNoteId++;
            handNote.Owner = this;
            handNote.AudioSourceOwner = PlaybackManager.Instance.GetAudioSourceForStem(PlaybackManager.Instance.CurrentSong.MainStem);
            handNote.StartPosition = GetGridStartPosition(horizontalIndex, 2);
            handNote.EndPosition = GetGridEndPosition(horizontalIndex, 2);
            handNote.ETA = -_globalNoteOffset;
            handNote.Gesture = HandGesture.OpenHand;

            handNote.ResetState();
            _activeNotes.Add(handNote.NoteId, handNote);
        }

        private void EnterNewSection(int mapDifficulty, bool freestyle, bool isBreak) {
            if (_currentDifficulty != mapDifficulty) {
                return;
            }

//            Debug.Log("Entered section that is freestyle=" + freestyle);

//            Debug.Log("Entered section that is break=" + isBreak);
            ChangeFreestyleMode(freestyle);

            if (isBreak) {
                _scoreboard.DisplayScoreboard();
            }
            else if (!isBreak) {
                _scoreboard.HideScoreboard();
            }
            ExecuteStreamSwitch();
        }

        private void EnterNewPhrase(int mapDifficulty, byte playerFlag) {
            if (_currentDifficulty != mapDifficulty) {
                return;
            }

            if (PlayerNumber > 0) {
                // Make sure that at least one player is connected who is unmuted
                if (CheckIfAnyPlayerUnmutedForFlag(playerFlag)) {
                    // Parse bitflag
                    int bitIndex = PlayerManager.MaxPlayers - PlayerNumber; // MSB is player 1, LSB is player n
                    _isMuted = (playerFlag & (1 << bitIndex)) != 0;

                    Debug.Log($"[LinePatternManager] Player {PlayerNumber} muted={_isMuted}");
                } else {
                    // No players are playing this, so everyone plays it
                    _isMuted = false;

                    Debug.Log($"[LinePatternManager] Player {PlayerNumber} not muted because the phrase doesn't include any active players");
                }
            } else {
                _isMuted = false;
            }

            // If there is some soloing and we're soloed (not muted), show the quadrant bounds
            if (Player != null) {
                QuadrantManager.Instance.GetQuadrantForPlayer(Player)
                    .SetBoundsVisible(playerFlag > 0 && !_isMuted);
            }
        }

        private static bool CheckIfAnyPlayerUnmutedForFlag(byte playerFlag) {
            for (int playerId = 1; playerId <= PlayerManager.MaxPlayers; ++playerId) {
                // Is the flag on?
                int bitIndex = PlayerManager.MaxPlayers - playerId; // MSB is player 1, LSB is player n
                bool muted = (playerFlag & (1 << bitIndex)) != 0;
                if (!muted && PlayerManager.Instance.Players.ContainsKey(playerId)) {
                    return false;
                }
            }

            return false;
        }

        private void ChangeFreestyleMode(bool newIsFreestyle) {
            if (newIsFreestyle == IsFreestyle) {
                return;
            }

            IsFreestyle = newIsFreestyle;

            if (newIsFreestyle) {
                if (GrindEmitter != null) GrindEmitter.ActivateFreestyleEmitters();
                if (GrindEmitter2 != null) GrindEmitter2.ActivateFreestyleEmitters();
            } else {
                if (GrindEmitter != null) GrindEmitter.DeactivateFreestyleEmitters();
                if (GrindEmitter2 != null) GrindEmitter2.DeactivateFreestyleEmitters();
            }
        }

        public void StreamUp() {
            _streamValue = 1;
        }

        public void StreamDown() {
            _streamValue = -1;
        }

        public void ExecuteStreamSwitch() {
//            Debug.Log("difficulty: " + _streamValue);
            if (_streamValue > 0) {
                _currentDifficulty = Mathf.Min(_currentDifficulty + 1, Beatmaps.Count - 1);
                _score.ResetStreamValues();
            }
            else if (_streamValue < 0) {
                _currentDifficulty = Mathf.Max(_currentDifficulty - 1, 0);
                _score.ResetStreamValues();
            }

            _streamValue = 0;

            _score.UpdateThreshold(_currentDifficulty);

            DifficultyChanged?.Invoke();
        }

        private Vector3 GetGridStartPosition(int gridX, int gridY) {
            Vector3 right = Vector3.Cross(Vector3.up, _laneEnd.position - _laneStart.position)
                .normalized;

            int flippedGridX = IsGridFlipped ? GridX - 1 - gridX : gridX;

            return _laneStart.position +
                   right * (_globalGridScaleX * flippedGridX) +
                   Vector3.up * (-_globalGridScaleY * gridY);
        }

        private Vector3 GetGridEndPosition(int gridX, int gridY) {
            Vector3 right = Vector3.Cross(Vector3.up, _laneEnd.position - _laneStart.position)
                .normalized;

            int flippedGridX = IsGridFlipped ? GridX - 1 - gridX : gridX;

            return _laneEnd.position +
                   right * (_globalGridScaleX * flippedGridX) +
                   Vector3.up * (-_globalGridScaleY * gridY);
        }

        public void FindInGrid(Vector3 position, out int x, out int y) {
            position -= _laneStart.position;
            x = (int) (Vector3.Dot(position, _laneStart.right) / _globalGridScaleX);
            y = (int) (Vector3.Dot(position, _laneStart.up) / -_globalGridScaleY);
        }

        public Vector3 FindInWorld(int x, int y) {
            return GetGridStartPosition(x, y);
        }

        public void ReportNoteFeedback(LinePatternNoteBase note, NoteFeedbackType feedbackType) {
            photonView.RPC(nameof(NoteFeedbackRpc), RpcTarget.Others,
                note.NoteId, feedbackType);
        }

        [PunRPC]
        private void NoteFeedbackRpc(int noteId, NoteFeedbackType feedbackType) {
            LinePatternNoteBase note;
            if (_activeNotes.TryGetValue(noteId, out note)) {
                switch (feedbackType) {
                    case NoteFeedbackType.Hit:
                        note.ShowHitFeedback();
                        break;
                    case NoteFeedbackType.Missed:
                        note.ShowFailFeedback();
                        break;
                }
            } else {
                Debug.LogWarning($"[LinePatternManager] Note {noteId} not found for {feedbackType}");
            }
        }

        public void DestroyNote(LinePatternNote note) {
            _activeNotes.Remove(note.NoteId);

            // TODO: pool lines too
            foreach (var line in note.GetComponentsInChildren<LinePatternLine>()) {
//                if (line.EndPos != Vector3.zero) {
//                    if (line.EndNote != null) {
//                        if (!line.EndNote.gameObject.activeSelf) {
//                            line.StartNote = null;
//                            line.transform.SetParent(null);
//                            continue;
//                        }
//                    } else {
//                        line.StartNote = null;
//                        line.transform.SetParent(null);
//                        continue;
//                    }
//                }
//                line.StartNote = null;
//                line.EndNote = null;
//                line.transform.SetParent(null);
                Destroy(line.gameObject);
            }

            // Return to pool
            _notePool.Release(note);
        }

        public void DestroyNote(LinePatternHand note) {
            foreach (var sprite in note._spriteRenderers) {
                sprite.enabled = true;
            }
            _activeNotes.Remove(note.NoteId);

            // Return to pool
            _handNotePool.Release(note);
        }
    }
}
